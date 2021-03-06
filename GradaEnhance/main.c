#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ファイルの存在確認
 * ----
 * fname : ファイル名
 * f()   : 存在フラグを返す（存在するなら1、存在しないなら0）
 */
int isFile(char *fname)
{
	FILE *fp;
	
	fp = fopen(fname, "rb");
	if (fp) fclose(fp);
	return (fp)? 1: 0;
}

/*
 * 画像をファイルから読み込む
 * ----
 * fname         : ファイル名
 * imgv          : 画像バッファ
 * width, height : 画像サイズ（横幅、高さ）
 * f()           : 成功フラグを返す（成功なら1、失敗なら0）
 */
int readRawFile(char *fname, unsigned short *imgv, int width, int height)
{
	int ret = 0, size = width * height;
	FILE *fp = NULL;
	
	for (;;) {
		if ((fp = fopen(fname, "rb")) == NULL) break;
		fread((void *)imgv, sizeof(unsigned short), size, fp);
		// 正常終了
		ret = 1;
		break;
	}
	if (fp) fclose(fp);
	return ret;
}

/*
 * 画像をファイルへ書き込む
 * ----
 * fname         : ファイル名
 * imgv          : 画像バッファ
 * width, height : 画像サイズ（横幅、高さ）
 * f()           : 成功フラグを返す（成功なら1、失敗なら0）
 */
int writeRawFile(char *fname, unsigned short *imgv, int width, int height)
{
	int ret = 0, size = width * height;
	FILE *fp = NULL;
	
	for (;;) {
		if ((fp = fopen(fname, "wb")) == NULL) break;
		fwrite(imgv, sizeof(unsigned short), size, fp);
		// 正常終了
		ret = 1;
		break;
	}
	if (fp) fclose(fp);
	return ret;
}

/*
 * 値をリミットを制御する
 * ----
 * value    : 値
 * min, max : 最小値、最大値
 * f()      : 正規化された値
 */
int normalize(int value, int min, int max)
{
	if (value < min) value = min;
	else if (max < value) value = max;
	return value;
}

/*
 * 階調変換する（コントラスト強調）
 * ----
 * jmgv, imgv    : 画像バッファ（出力、入力）
 * width, height : 画像サイズ（横幅、高さ）
 * a, b          : 階調の範囲（下限、上限）
 * c, d          : ウィンドウの範囲（下限、上限）
 */
int gradation(unsigned short *jmgv, unsigned short *imgv,
			  int width, int height, int a, int b, int c, int d)
{
	int size, i, value;
	
	size = width * height;
	for (i = 0; i < size; i++) {
		value = (double)(imgv[i]-c)/(d-c)*(b-a)+a;
		jmgv[i] = normalize(value, a, b);
	}
	return 1;
}

/*
 * 画像処理する
 * ----
 * dst, src      : ファイル名（出力、入力）
 * width, height : 画像サイズ（横幅、高さ）
 * minv, maxv    : ウィンドウ（下限、上限）
 * f()           : 成功フラグを返す（成功なら1、失敗なら0）
 */
int process(char *dst, char *src, int width, int height, int minv, int maxv)
{
	int ret = 0, size, class_num;
	unsigned short *imgv = NULL, *jmgv = NULL;
	
	for (;;) {
		// データ領域を確保する
		size = width * height;
		if ((imgv = (unsigned short *)malloc(sizeof(unsigned short)*size)) == NULL) break;
		if ((jmgv = (unsigned short *)malloc(sizeof(unsigned short)*size)) == NULL) break;
		// 階調処理する
		if (readRawFile(src, imgv, width, height) == 0) break;
		if ((class_num = maxv - minv + 1) <= 0) break;
		gradation(jmgv, imgv, width, height, 0, 255, minv, maxv);
		if (writeRawFile(dst, jmgv, width, height) == 0) break;
		// 正常終了
		ret = 1;
		break;
	}
	// データ領域を解放する
	if (jmgv) free(jmgv);
	if (imgv) free(imgv);
	return ret;
}

int main(void)
{
	char dst[256], src[256];
	int flag, width, height, minv, maxv;
	
	printf("入力ファイル： ");		scanf("%s", src);
	if (isFile(src) == 0) {
		printf("ファイル[%s]が見つかりません。\n", src);
		return 0;
	}
	printf("横幅（ピクセル）： ");	scanf("%d", &width);
	printf("高さ（ピクセル）： ");	scanf("%d", &height);
	printf("ウインドウ下限値： ");	scanf("%d", &minv);
	printf("ウィンドウ上限値： ");	scanf("%d", &maxv);
	printf("出力ファイル： ");		scanf("%s", dst);
	
	flag = process(dst, src, width, height, minv, maxv);
	printf((flag)? ">成功しました。\n": ">失敗しました。\n");
	return 0;
}
