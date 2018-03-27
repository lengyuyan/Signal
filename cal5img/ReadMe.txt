GetState5
计算五张图片的DeltaZ、U、V

参考confluence上的经验值，我们认为阈值设置为0.0045与0.19时：
最终收敛结果始终保持在最佳结果的±0.006以内，详可分析数据。
因此，在初步收敛后，采集收敛位置 -0.006， -0.003， 0， 0.003， 0.006共五个位置的image
从小到大，一次输入getState5

DeltaZ：找出五张图中，center位置laplace最大的位置，求出Delta，无拟合过程
DeltaU、V： 
	以U举例，找出left，center，right的最大值位置，进行一次拟合，求出U