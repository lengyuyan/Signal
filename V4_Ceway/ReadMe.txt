主程序修改的地方：
1.Main函数中，
  	double deltaZ_p = p_num - ori_num;
  	double deltaZ_m = m_num - ori_num;
  需要进行修改！
  	double adjust_z = state.deltaZ;
	double adjust_u = state.x_angle * -1;
	double adjust_v = state.y_angle * -1;
  维持不变
2.state函数增加了参数scale

3.很多参数设置直接设定在了state里，如有需要，可以挪移至主函数

4.增加了选框判断功能，单边判断功能，体现为cout，请整合至运行程序


请注意，此版本的Adjust正负号与v1正好相反