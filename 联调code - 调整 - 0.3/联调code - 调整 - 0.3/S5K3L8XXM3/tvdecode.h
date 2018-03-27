#ifndef _TV_DECODE_H_
#define _TV_DECODE_H_

void EdageDirectLineAvg(unsigned char *pDisplayBuffer, int width, int height);
void Bob(unsigned char *pDisplayBuffer, int width, int height);
void weave(unsigned char *pDisplayBuffer, int width, int height);
void DynamicModule(unsigned char *pDisplayBuffer, int width, int height);
////////////////动态ELA///////////////////////////
void DynamicELA(unsigned char *pDisplayBuffer, int width, int height);// 推荐
void ModifiedELA(unsigned char *pDisplayBuffer, int width, int height);
//***********************边沿自适应***********************//
void EdageAdption(unsigned char *pDisplayBuffer, int width, int height);

#endif