/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   bhy
Version:    1.0
Date:     2024-08-26 9:13:56
Description: 质保书处置
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include<regex>

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(qmts0r_pro1)

int f_qmts0r_pro1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString ana_time_tg = "";
	CString v_order_no = "";
	CString v_heat_no = "";
	CString v_now_row = "";
	CString v_operate = "";
	CString datetime = CDateTime::Now().ToString("yyyy-MM-dd");
	//CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString slab_cut_time = " ";
	CString slab_cut_time1 = " ";
	CString image = " ";

	CString last_elm = "";
	CString v_elm_tc = "";
	CString v_elm_min_tc = "";
	CString v_elm_max_tc = "";
	CString v_elm_value = "";
	CString v_manual_flag = "";
	CString v_userid = s.userid;
	CString v_check_time = "";

	CModel qmts0r05("TQMTS0R05");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_last(conn);


	try
	{
		//--------------------------------
		//获取传入参数
		v_operate = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		v_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();
		v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		v_now_row = bcls_rec->Tables[0].Rows[0]["NOW_ROW"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "v_operate =[{0}],v_heat_no=[{1}]", v_operate, v_heat_no);
		cmd_inq.SetCommandText(" select MAX(SLAB_CUT_TIME) AS SLAB_CUT_TIME from ( "
			" select SLAB_CUT_TIME, HEAT_NO from TMMSM01 "
			" union "
			" select SLAB_CUT_TIME, HEAT_NO from HMMSM01 "
			" ) where HEAT_NO = '" + v_heat_no + "' ");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read()){
			Log::Info("", __FUNCTION__, "slab_cut_time =[{0}]", cmd_inq.GetString(1));
			if (cmd_inq.GetString(1) != ""){
				slab_cut_time = cmd_inq.GetDateTime(1).ToString("yyyy-MM-dd");
				slab_cut_time1 = cmd_inq.GetDateTime(1).AddDays(+1).ToString("yyyy-MM-dd");
			}
		}
		cmd_inq.Close();
		Log::Info("", __FUNCTION__, "slab_cut_time =[{0}]", slab_cut_time);

		Log::Info("", __FUNCTION__, "userid =[{0}]", v_userid);
	
		if (v_userid == "HJ2148"){
			image = "http://10.162.72.16:10004/DiBei/HJ2148.jpg";
		}
		else if (v_userid == "HM1805")
		{
			image = "http://10.162.72.16:10004/DiBei/HM1805.jpg";
		}
		else if (v_userid == "HK6475")
		{
			image = "http://10.162.72.16:10004/DiBei/HK6475.jpg";
		}
		else if (v_userid == "HC2406")
		{
			image = "http://10.162.72.16:10004/DiBei/HC2406.jpg";
		}
		else if (v_userid == "HK6779")
		{
			image = "http://10.162.72.16:10004/DiBei/HK6779.jpg";
		}
		else if (v_userid == "HK1608")
		{
			image = "http://10.162.72.16:10004/DiBei/HK1608.jpg";
		}
		else if (v_userid == "HK7927")
		{
			image = "http://10.162.72.16:10004/DiBei/HK7927.jpg";
		}
		else if (v_userid == "HM5335")
		{
			image = "http://10.162.72.16:10004/DiBei/HM5335.png";
		}
		//2025.09.19 加特钢
		else if (v_userid == "HC1744")
		{
			image = "http://10.162.72.16:10004/DiBei/HC1744.jpg";
		}
		else if (v_userid == "HB9215")
		{
			image = "http://10.162.72.16:10004/DiBei/HB9215.jpg";
		}
		else if (v_userid == "HM7161")
		{
			image = "http://10.162.72.16:10004/DiBei/HM7161.jpg";
		}
		else if (v_userid == "HK1958")
		{
			image = "http://10.162.72.16:10004/DiBei/HK1958.jpg";
		}
		else if (v_userid == "HE5200")
		{
			image = "http://10.162.72.16:10004/DiBei/HE5200.jpg";
		}
		else if (v_userid == "HJ4611")
		{
			image = "http://10.162.72.16:10004/DiBei/HJ4611.jpg";
		}
		else if (v_userid == "03366934")
		{
			image = "http://10.162.72.16:10004/DiBei/HJ4611.jpg";
		}
		//
		else{
			image = "http://10.162.72.16:10004/DiBei/MR.png";
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			qmts0r05.Reset();
			qmts0r05.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_operate == "A")
			{
				/*if (doFlag < 0)
				{
				throw CApplicationException(-1, s.msg, log.Location);
				}*/
				qmts0r05["AYL_FLAG"] = "1";
				qmts0r05["AYL_MAKE"] = s.userid;
				qmts0r05["AYL_TIME"] = datetime;
				qmts0r05["AYL_IMAGE"] = image;
				Log::Info("", __FUNCTION__, "炉号开头 =[{0}]", qmts0r05["HEAT_NO"].ToString().Substring(0, 1));
				if (qmts0r05["HEAT_NO"].ToString().Substring(0, 1) == "A" || qmts0r05["HEAT_NO"].ToString().Substring(0, 1) == "B")
				{
					if (slab_cut_time != " "){
						qmts0r05["ANA_TIME"] = slab_cut_time;
					}
					else{
						qmts0r05["ANA_TIME"] = datetime;
					}
				}
				else if (qmts0r05["HEAT_NO"].ToString().Substring(0, 1) == "C")
				{
					//2025.10.14 特钢不锈钢成品成分表,根据炉号取最新一条的分析时间
					cmd_inq.SetCommandText(" SELECT ANA_TIME_TG FROM qmts0rxy_tg "
						"where HEAT_NO = '" + v_heat_no + "' "
						);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read()){
						Log::Info("", __FUNCTION__, "查询结果ANA_TIME_TG =[{0}]", cmd_inq.GetString(1));
						if (cmd_inq.GetString(1) != ""){
							ana_time_tg = cmd_inq.GetString(1);
						}
					}
					cmd_inq.Close();
					if (ana_time_tg != "")
					{
						qmts0r05["ANA_TIME"] = ana_time_tg;
						Log::Info("", __FUNCTION__, "ANA_TIME_TG =[{0}]", ana_time_tg);
					}
					else{
						qmts0r05["ANA_TIME"] = datetime;
						Log::Info("", __FUNCTION__, "111ANA_TIME_TG =[{0}]", ana_time_tg);
					}
				}
				
				Log::Info("", __FUNCTION__, "image =[{0}", image);
				Log::Trace("","","111");
				qmts0r05.Update("AYL_FLAG,AYL_MAKE,AYL_TIME,ANA_TIME,AYL_IMAGE", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "C")
			{
				qmts0r05["CHECK_FLAG"] = "1";
				qmts0r05["CHECK_MAKE"] = s.userid;
				qmts0r05["CHECK_DATE"] = datetime;
				qmts0r05["CHECK_IMAGE"] = image;
				if (qmts0r05["HEAT_NO"].ToString().Substring(0, 1) == "A" || qmts0r05["HEAT_NO"].ToString().Substring(0, 1) == "B")
				{
					if (slab_cut_time != " "){
						qmts0r05["CHECK_TIME"] = slab_cut_time;
					}
					else{
						qmts0r05["CHECK_TIME"] = datetime;
					}
				}
				else if (qmts0r05["HEAT_NO"].ToString().Substring(0, 1) == "C")
				{
					//2025.10.14 特钢不锈钢成品成分表,根据炉号取最新一条的审核时间
					cmd_inq.SetCommandText(" SELECT ANA_TIME_TG FROM qmts0rxy_tg "
						"where HEAT_NO = '" + v_heat_no + "' "
						);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read()){
						Log::Info("", __FUNCTION__, "查询结果ANA_TIME_TG =[{0}]", cmd_inq.GetString(1));
						if (cmd_inq.GetString(1) != ""){
							ana_time_tg = cmd_inq.GetString(1);
						}
					}
					cmd_inq.Close();
					if (ana_time_tg != "")
					{
						qmts0r05["CHECK_TIME"] = ana_time_tg;
						Log::Info("", __FUNCTION__, "CHECK_TIME_TG =[{0}]", ana_time_tg);
					}
					else{
						qmts0r05["CHECK_TIME"] = datetime;
						Log::Info("", __FUNCTION__, "111CHECK_TIME_TG =[{0}]", ana_time_tg);
					}
				}
				Log::Info("", __FUNCTION__, "image =[{0}", image);
				qmts0r05.Update("CHECK_FLAG,CHECK_MAKE,CHECK_DATE,CHECK_TIME,CHECK_IMAGE", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "D")
			{
				qmts0r05["DECIDE_CODE"] = "1";
				qmts0r05["DECIDER"] = s.userid;
				qmts0r05["JUDGE_TIME"] = datetime;
				qmts0r05["DECIDE_IMAGE"] = image;
				//if (slab_cut_time != " "){
				//	//CDateTime::Now().AddDays(+1).ToString("yyyy-MM-dd");
				//	qmts0r05["DECIDE_TIME"] = slab_cut_time1;
				//	qmts0r05["DECIDE_TIME"] = qmts0r05["CHECK_TIME"];
				//	Log::Info("", __FUNCTION__, "DECIDE_TIME =[{0}],v_heat_no=[{1}]", slab_cut_time, v_heat_no);
				//}
				cmd_inq.SetCommandText(" select CHECK_TIME from TQMTS0R05 "
					"where HEAT_NO = '" + v_heat_no + "' "
					"AND ORDER_NO = '" + v_order_no + "'"
					"AND NOW_ROW = '" + v_now_row + "'"
					);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read()){
					Log::Info("", __FUNCTION__, "v_check_time =[{0}]", cmd_inq.GetString(1));
					if (cmd_inq.GetString(1) != ""){
						v_check_time = cmd_inq.GetString(1);
					}
				}
				cmd_inq.Close();
				//2024.12.31 修改判定时间默认和修改审核时间一致
				if (v_check_time != " ")
				{
					qmts0r05["DECIDE_TIME"] = v_check_time;
					Log::Info("", __FUNCTION__, "CHECK_TIME =[{0}]", v_check_time);
				}
				else{
					qmts0r05["DECIDE_TIME"] = datetime;
					Log::Info("", __FUNCTION__, "111CHECK_TIME =[{0}]", v_check_time);
				}
				Log::Info("", __FUNCTION__, "image =[{0}", image);
				qmts0r05.Update("DECIDE_CODE,DECIDER,JUDGE_TIME,DECIDE_TIME,DECIDE_IMAGE", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "U7")
			{
				Log::Info("", __FUNCTION__, "ANA_TIME =[{0}", qmts0r05["ANA_TIME"].ToString());
				qmts0r05.Update("ANA_TIME", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "U8")
			{
				//qmts0r05["CHECK_TIME"] = datetime;
				qmts0r05.Update("CHECK_TIME", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "U9")
			{
				//qmts0r05["DECIDE_TIME"] = datetime;

				qmts0r05.Update("DECIDE_TIME", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "U10")
			{
				//qmts0r05["DECIDE_TIME"] = datetime;

				qmts0r05.Update("DESCRIPTION", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "U11")
			{
				qmts0r05.Update("REMARK", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "U12")
			{
				Log::Trace("", "", "222");

				sqlstr = "SELECT CASE WHEN T.ELM_01_TC = ' ' THEN '01' "
					" WHEN T.ELM_02_TC = ' ' THEN '02' "
					" WHEN T.ELM_03_TC = ' ' THEN '03' "
					" WHEN T.ELM_04_TC = ' ' THEN '04' "
					" WHEN T.ELM_05_TC = ' ' THEN '05' "
					" WHEN T.ELM_06_TC = ' ' THEN '06' " 
					" WHEN T.ELM_07_TC = ' ' THEN '07' "
					" WHEN T.ELM_08_TC = ' ' THEN '08' "
					" WHEN T.ELM_09_TC = ' ' THEN '09' "
					" WHEN T.ELM_10_TC = ' ' THEN '10' "
					" WHEN T.ELM_11_TC = ' ' THEN '11' "
					" WHEN T.ELM_12_TC = ' ' THEN '12' "
					" WHEN T.ELM_13_TC = ' ' THEN '13' "
					" WHEN T.ELM_14_TC = ' ' THEN '14' "
					" WHEN T.ELM_15_TC = ' ' THEN '15' "
					" WHEN T.ELM_16_TC = ' ' THEN '16' "
					" WHEN T.ELM_17_TC = ' ' THEN '17' "
					" WHEN T.ELM_18_TC = ' ' THEN '18' "
					" WHEN T.ELM_19_TC = ' ' THEN '19' "
					" WHEN T.ELM_20_TC = ' ' THEN '20' "
					" WHEN T.ELM_21_TC = ' ' THEN '21' "
					" WHEN T.ELM_22_TC = ' ' THEN '22' "
					" WHEN T.ELM_23_TC = ' ' THEN '23' "
					" WHEN T.ELM_24_TC = ' ' THEN '24' "
					" WHEN T.ELM_25_TC = ' ' THEN '25' "
					" WHEN T.ELM_26_TC = ' ' THEN '26' "
					" WHEN T.ELM_27_TC = ' ' THEN '27' "
					" WHEN T.ELM_28_TC = ' ' THEN '28' "
					" WHEN T.ELM_29_TC = ' ' THEN '29' "
					" ELSE '30' END AS LAST_ELM "
					" FROM TQMTS0R05 T WHERE 1 = 1 AND HEAT_NO = '" + v_heat_no + "' "
					" AND ORDER_NO = '" + v_order_no + "'"
					" AND NOW_ROW = '" + v_now_row + "'"
					;
				cmd_inq_last.SetCommandText(sqlstr);
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq_last.ExecuteReader();
				if (cmd_inq_last.Read())
				{
					last_elm = cmd_inq_last.GetString(1);
					Log::Info("", __FUNCTION__, "last_elm =[{0}]", last_elm);

					Log::Trace("", "", "333");
					if (last_elm == "01")
					{
						Log::Trace("", "", "444");

						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG01"] = "1";
						qmts0r05["ELM_01_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_01_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_01_MIN_TC"] = " ";
						}
						qmts0r05["ELM_01_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_01"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_01"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						qmts0r05["EXP_01_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG01,ELM_01_TC,ELM_01_MIN_TC,ELM_01_MAX_TC,ELM_VALUE_01,EXP_01_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "02")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG02"] = "1";
						qmts0r05["ELM_02_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_02_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_02_MIN_TC"] = " ";
						}						
						qmts0r05["ELM_02_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_02"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_02"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						qmts0r05["EXP_02_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG02,ELM_02_TC,ELM_02_MIN_TC,ELM_02_MAX_TC,ELM_VALUE_02,EXP_02_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "03")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG03"] = "1";
						qmts0r05["ELM_03_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_03_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_03_MIN_TC"] = " ";
						}
						qmts0r05["ELM_03_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_03"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_03"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						qmts0r05["EXP_03_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG03,ELM_03_TC,ELM_03_MIN_TC,ELM_03_MAX_TC,ELM_VALUE_03,EXP_03_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "04")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG04"] = '1';
						qmts0r05["ELM_04_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_04_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_04_MIN_TC"] = " ";
						}
						qmts0r05["ELM_04_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_04"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_04"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						qmts0r05["EXP_04_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG04,ELM_04_TC,ELM_04_MIN_TC,ELM_04_MAX_TC,ELM_VALUE_04,EXP_04_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "05")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG05"] = '1';
						qmts0r05["ELM_05_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_05_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_05_MIN_TC"] = " ";
						}
						qmts0r05["ELM_05_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_05"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_05"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						qmts0r05["EXP_05_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG05,ELM_05_TC,ELM_05_MIN_TC,ELM_05_MAX_TC,ELM_VALUE_05,EXP_05_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "06")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG06"] = '1';
						qmts0r05["ELM_06_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_06_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_06_MIN_TC"] = " ";
						}
						qmts0r05["ELM_06_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_06"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_06"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_06_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_06_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG06,ELM_06_TC,ELM_06_MIN_TC,ELM_06_MAX_TC,ELM_VALUE_06,EXP_06_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "07")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG07"] = '1';
						qmts0r05["ELM_07_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_07_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_07_MIN_TC"] = " ";
						}
						qmts0r05["ELM_07_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_07"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_07"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_07_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_07_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG07,ELM_07_TC,ELM_07_MIN_TC,ELM_07_MAX_TC,ELM_VALUE_07,EXP_07_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "08")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG08"] = '1';
						qmts0r05["ELM_08_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_08_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_08_MIN_TC"] = " ";
						}
						qmts0r05["ELM_08_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_08"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_08"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_08_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_08_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG08,ELM_08_TC,ELM_08_MIN_TC,ELM_08_MAX_TC,ELM_VALUE_08,EXP_08_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "09")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG09"] = '1';
						qmts0r05["ELM_09_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_09_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_09_MIN_TC"] = " ";
						}
						qmts0r05["ELM_09_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_09"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_09"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_09_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_09_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG09,ELM_09_TC,ELM_09_MIN_TC,ELM_09_MAX_TC,ELM_VALUE_09,EXP_09_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "10")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG10"] = '1';
						qmts0r05["ELM_10_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_10_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_10_MIN_TC"] = " ";
						}
						qmts0r05["ELM_10_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_10"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_10"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_10_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_10_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG10,ELM_10_TC,ELM_10_MIN_TC,ELM_10_MAX_TC,ELM_VALUE_10,EXP_10_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "11")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG11"] = '1';
						qmts0r05["ELM_11_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_11_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_11_MIN_TC"] = " ";
						}
						qmts0r05["ELM_11_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_11"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_11"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_11_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_11_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG11,ELM_11_TC,ELM_11_MIN_TC,ELM_11_MAX_TC,ELM_VALUE_11,EXP_11_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "12")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG12"] = '1';
						qmts0r05["ELM_12_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_12_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_12_MIN_TC"] = " ";
						}
						qmts0r05["ELM_12_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_12"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_12"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_12_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_12_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG12,ELM_12_TC,ELM_12_MIN_TC,ELM_12_MAX_TC,ELM_VALUE_12,EXP_12_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "13")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG13"] = '1';
						qmts0r05["ELM_13_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_13_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_13_MIN_TC"] = " ";
						}
						qmts0r05["ELM_13_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_13"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_13"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						Log::Info("", __FUNCTION__, "第一位 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) );
						Log::Info("", __FUNCTION__, "111 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength());
						Log::Info("", __FUNCTION__, "222 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength());

						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_13_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_13_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG13,ELM_13_TC,ELM_13_MIN_TC,ELM_13_MAX_TC,ELM_VALUE_13,EXP_13_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "14")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG14"] = '1';
						qmts0r05["ELM_14_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_14_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_14_MIN_TC"] = " ";
						}
						qmts0r05["ELM_14_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_14"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_14"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_14_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_14_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG14,ELM_14_TC,ELM_14_MIN_TC,ELM_14_MAX_TC,ELM_VALUE_14,EXP_14_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "15")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG15"] = '1';
						qmts0r05["ELM_15_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_15_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_15_MIN_TC"] = " ";
						}
						qmts0r05["ELM_15_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_15"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_15"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_15_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_15_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG15,ELM_15_TC,ELM_15_MIN_TC,ELM_15_MAX_TC,ELM_VALUE_15,EXP_15_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "16")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG16"] = '1';
						qmts0r05["ELM_16_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_16_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_16_MIN_TC"] = " ";
						}
						qmts0r05["ELM_16_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_16"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_16"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_16_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_16_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG16,ELM_16_TC,ELM_16_MIN_TC,ELM_16_MAX_TC,ELM_VALUE_16,EXP_16_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "17")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG17"] = '1';
						qmts0r05["ELM_17_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_17_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_17_MIN_TC"] = " ";
						}
						qmts0r05["ELM_17_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_17"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_17"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_17_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_17_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG17,ELM_17_TC,ELM_17_MIN_TC,ELM_17_MAX_TC,ELM_VALUE_17,EXP_17_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "18")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG18"] = '1';
						qmts0r05["ELM_18_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_18_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_18_MIN_TC"] = " ";
						}
						qmts0r05["ELM_18_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_18"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_18"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_18_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_18_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG18,ELM_18_TC,ELM_18_MIN_TC,ELM_18_MAX_TC,ELM_VALUE_18,EXP_18_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "19")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG19"] = '1';
						qmts0r05["ELM_19_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_19_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_19_MIN_TC"] = " ";
						}
						qmts0r05["ELM_19_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_19"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_19"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_19_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_19_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG19,ELM_19_TC,ELM_19_MIN_TC,ELM_19_MAX_TC,ELM_VALUE_19,EXP_19_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "20")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG20"] = '1';
						qmts0r05["ELM_20_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_20_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_20_MIN_TC"] = " ";
						}
						qmts0r05["ELM_20_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_20"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_20"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_20_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_20_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG20,ELM_20_TC,ELM_20_MIN_TC,ELM_20_MAX_TC,ELM_VALUE_20,EXP_20_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "21")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG21"] = '1';
						qmts0r05["ELM_21_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_21_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_21_MIN_TC"] = " ";
						}
						qmts0r05["ELM_21_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_21"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_21"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_21_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_21_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG21,ELM_21_TC,ELM_21_MIN_TC,ELM_21_MAX_TC,ELM_VALUE_21,EXP_21_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "22")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG22"] = '1';
						qmts0r05["ELM_22_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_22_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_22_MIN_TC"] = " ";
						}
						qmts0r05["ELM_22_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_22"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_22"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_22_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_22_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG22,ELM_22_TC,ELM_22_MIN_TC,ELM_22_MAX_TC,ELM_VALUE_22,EXP_22_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "23")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG23"] = '1';
						qmts0r05["ELM_23_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_23_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_23_MIN_TC"] = " ";
						}
						qmts0r05["ELM_23_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_23"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_23"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_23_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_23_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG23,ELM_23_TC,ELM_23_MIN_TC,ELM_23_MAX_TC,ELM_VALUE_23,EXP_23_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "24")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG24"] = '1';
						qmts0r05["ELM_24_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_24_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_24_MIN_TC"] = " ";
						}
						qmts0r05["ELM_24_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_24"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_24"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_24_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_24_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG24,ELM_24_TC,ELM_24_MIN_TC,ELM_24_MAX_TC,ELM_VALUE_24,EXP_24_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "25")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG25"] = '1';
						qmts0r05["ELM_25_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_25_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_25_MIN_TC"] = " ";
						}
						qmts0r05["ELM_25_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_25"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_25"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_25_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_25_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG25,ELM_25_TC,ELM_25_MIN_TC,ELM_25_MAX_TC,ELM_VALUE_25,EXP_25_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "26")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG26"] = '1';
						qmts0r05["ELM_26_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_26_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_26_MIN_TC"] = " ";
						}
						qmts0r05["ELM_26_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_26"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_26"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_26_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_26_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG26,ELM_26_TC,ELM_26_MIN_TC,ELM_26_MAX_TC,ELM_VALUE_26,EXP_26_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "27")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG27"] = '1';
						qmts0r05["ELM_27_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_27_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_27_MIN_TC"] = " ";
						}
						qmts0r05["ELM_27_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_27"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_27"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_27_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_27_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG27,ELM_27_TC,ELM_27_MIN_TC,ELM_27_MAX_TC,ELM_VALUE_27,EXP_27_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "28")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG28"] = '1';
						qmts0r05["ELM_28_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_28_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_28_MIN_TC"] = " ";
						}
						qmts0r05["ELM_28_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_28"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_28"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_28_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_28_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG28,ELM_28_TC,ELM_28_MIN_TC,ELM_28_MAX_TC,ELM_VALUE_28,EXP_28_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "29")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG29"] = '1';
						qmts0r05["ELM_29_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_29_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_29_MIN_TC"] = " ";
						}
						qmts0r05["ELM_29_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_29"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_29"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_29_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_29_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG29,ELM_29_TC,ELM_29_MIN_TC,ELM_29_MAX_TC,ELM_VALUE_29,EXP_29_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					if (last_elm == "30")
					{
						qmts0r05["REC_REVISOR"] = s.userid;
						qmts0r05["REC_REVISE_TIME"] = datetime;
						qmts0r05["MANUAL_FLAG30"] = '1';
						qmts0r05["ELM_30_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_TC"].ToString();
						if (bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString() != "")
						{
							qmts0r05["ELM_30_MIN_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MIN_TC"].ToString().Trim();
						}
						else
						{
							qmts0r05["ELM_30_MIN_TC"] = " ";
						}
						qmts0r05["ELM_30_MAX_TC"] = bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim();
						qmts0r05["ELM_VALUE_30"] = bcls_rec->Tables[0].Rows[0]["ELM_VALUE"].ToString().Trim();
						//实际值输入要求是英文字符
						string dest = string((const char*)qmts0r05["ELM_VALUE_30"].ToString().Trim());
						Log::Trace("", "", dest);
						regex pattern("[\u4e00-\u9fa5]");
						bool is_match = regex_search(dest, pattern);
						if (is_match)
						{
							strcpy(s.msg, "保存失败！实际值中的所有符号须为英文字符");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						//手动增加成分信息的修约小数位数
						if (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().Trim().Substring(0, 1) == "<"){
							qmts0r05["EXP_30_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 2);
						}
						else
						{
							qmts0r05["EXP_30_TC"] = (bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
							Log::Info("", __FUNCTION__, "last_elm小数位数 =[{0}]", bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToString().GetLength() - bcls_rec->Tables[0].Rows[0]["ELM_MAX_TC"].ToDecimal().Round(0).ToString().GetLength() - 1);
						}
						cmd_inq_last.Close();
						qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,MANUAL_FLAG30,ELM_30_TC,ELM_30_MIN_TC,ELM_30_MAX_TC,ELM_VALUE_30,EXP_30_TC", "HEAT_NO,ORDER_NO,NOW_ROW");
					}
					//qmts0r05["MANUAL_FLAG"] = '1';
						//MANUAL_FLAGXX = '1'
						//ELM_XX_TC
						//ELM_VALUE_XX
						//ELM_XX_MIN_TC
						//ELM_XX_MAX_TC
				}
				//cmd_inq.Close();
				//qmts0r05.Update("*", "HEAT_NO,ORDER_NO,NOW_ROW");
			}
			else if (v_operate == "D12")
			{
				Log::Trace("", "", "D12");

				CString elm_row = bcls_rec->Tables[0].Rows[0]["ELM_ROW"].ToString().Trim();
				
				qmts0r05["REC_REVISOR"] = s.userid;
				qmts0r05["REC_REVISE_TIME"] = datetime;
				qmts0r05["ELM_" + elm_row + "_TC"] = " ";
				Log::Info("", __FUNCTION__, "删除元素 =[{0}]", qmts0r05["ELM_" + elm_row + "_TC"].ToString());
				qmts0r05["ELM_" + elm_row + "_MIN_TC"] = " ";
				qmts0r05["ELM_" + elm_row + "_MAX_TC"] = " ";
				qmts0r05["ELM_VALUE_" + elm_row] = " ";
				if (elm_row == "01")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_01_TC,ELM_01_MIN_TC,ELM_01_MAX_TC,ELM_VALUE_01", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "02")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_02_TC,ELM_02_MIN_TC,ELM_02_MAX_TC,ELM_VALUE_02", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "03")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_03_TC,ELM_03_MIN_TC,ELM_03_MAX_TC,ELM_VALUE_03", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "04")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_04_TC,ELM_04_MIN_TC,ELM_04_MAX_TC,ELM_VALUE_04", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "05")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_05_TC,ELM_05_MIN_TC,ELM_05_MAX_TC,ELM_VALUE_05", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "06")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_06_TC,ELM_06_MIN_TC,ELM_06_MAX_TC,ELM_VALUE_06", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "07")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_07_TC,ELM_07_MIN_TC,ELM_07_MAX_TC,ELM_VALUE_07", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "08")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_08_TC,ELM_08_MIN_TC,ELM_08_MAX_TC,ELM_VALUE_08", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "09")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_09_TC,ELM_09_MIN_TC,ELM_09_MAX_TC,ELM_VALUE_09", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "10")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_10_TC,ELM_10_MIN_TC,ELM_10_MAX_TC,ELM_VALUE_10", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "11")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_11_TC,ELM_11_MIN_TC,ELM_11_MAX_TC,ELM_VALUE_11", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "12")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_12_TC,ELM_12_MIN_TC,ELM_12_MAX_TC,ELM_VALUE_12", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "13")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_13_TC,ELM_13_MIN_TC,ELM_13_MAX_TC,ELM_VALUE_13", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "14")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_14_TC,ELM_14_MIN_TC,ELM_14_MAX_TC,ELM_VALUE_14", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "15")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_15_TC,ELM_15_MIN_TC,ELM_15_MAX_TC,ELM_VALUE_15", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "16")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_16_TC,ELM_16_MIN_TC,ELM_16_MAX_TC,ELM_VALUE_16", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "17")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_17_TC,ELM_17_MIN_TC,ELM_17_MAX_TC,ELM_VALUE_17", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "18")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_18_TC,ELM_18_MIN_TC,ELM_18_MAX_TC,ELM_VALUE_18", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "19")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_19_TC,ELM_19_MIN_TC,ELM_19_MAX_TC,ELM_VALUE_19", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "20")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_20_TC,ELM_20_MIN_TC,ELM_20_MAX_TC,ELM_VALUE_20", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "21")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_21_TC,ELM_21_MIN_TC,ELM_21_MAX_TC,ELM_VALUE_21", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "22")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_22_TC,ELM_22_MIN_TC,ELM_22_MAX_TC,ELM_VALUE_22", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "23")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_23_TC,ELM_23_MIN_TC,ELM_23_MAX_TC,ELM_VALUE_23", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "24")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_24_TC,ELM_24_MIN_TC,ELM_24_MAX_TC,ELM_VALUE_24", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "25")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_25_TC,ELM_25_MIN_TC,ELM_25_MAX_TC,ELM_VALUE_25", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "26")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_26_TC,ELM_26_MIN_TC,ELM_26_MAX_TC,ELM_VALUE_26", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "27")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_27_TC,ELM_27_MIN_TC,ELM_27_MAX_TC,ELM_VALUE_27", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "28")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_28_TC,ELM_28_MIN_TC,ELM_28_MAX_TC,ELM_VALUE_28", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "29")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_29_TC,ELM_29_MIN_TC,ELM_29_MAX_TC,ELM_VALUE_29", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
				if (elm_row == "30")
				{
					qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_30_TC,ELM_30_MIN_TC,ELM_30_MAX_TC,ELM_VALUE_30", "HEAT_NO,ORDER_NO,NOW_ROW");
				}
			}
			else if (v_operate == "U13")
			{
				Log::Trace("", "", "修改规定值");
				qmts0r05["REC_REVISOR"] = s.userid;
				qmts0r05["REC_REVISE_TIME"] = datetime;
				qmts0r05.Update("REC_REVISOR,REC_REVISE_TIME,ELM_01_MIN_TC,ELM_01_MAX_TC,ELM_02_MIN_TC,ELM_02_MAX_TC,ELM_03_MIN_TC,ELM_03_MAX_TC,ELM_04_MIN_TC,ELM_04_MAX_TC,ELM_05_MIN_TC,ELM_05_MAX_TC,ELM_06_MIN_TC,ELM_06_MAX_TC,ELM_07_MIN_TC,ELM_07_MAX_TC,ELM_08_MIN_TC,ELM_08_MAX_TC,ELM_09_MIN_TC,ELM_09_MAX_TC,ELM_10_MIN_TC,ELM_10_MAX_TC,ELM_11_MIN_TC,ELM_11_MAX_TC,ELM_12_MIN_TC,ELM_12_MAX_TC,ELM_13_MIN_TC,ELM_13_MAX_TC,ELM_14_MIN_TC,ELM_14_MAX_TC,ELM_15_MIN_TC,ELM_15_MAX_TC,ELM_16_MIN_TC,ELM_16_MAX_TC,ELM_17_MIN_TC,ELM_17_MAX_TC,ELM_18_MIN_TC,ELM_18_MAX_TC,ELM_19_MIN_TC,ELM_19_MAX_TC,ELM_20_MIN_TC,ELM_20_MAX_TC,ELM_21_MIN_TC,ELM_21_MAX_TC,ELM_22_MIN_TC,ELM_22_MAX_TC,ELM_23_MIN_TC,ELM_23_MAX_TC,ELM_24_MIN_TC,ELM_24_MAX_TC,ELM_25_MIN_TC,ELM_25_MAX_TC,ELM_26_MIN_TC,ELM_26_MAX_TC,ELM_27_MIN_TC,ELM_27_MAX_TC,ELM_28_MIN_TC,ELM_28_MAX_TC,ELM_29_MIN_TC,ELM_29_MAX_TC,ELM_30_MIN_TC,ELM_30_MAX_TC", "HEAT_NO,ORDER_NO,NOW_ROW");

			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
