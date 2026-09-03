/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   bhy
Version:    1.0
Date:     2024-08-26 9:13:56
Description: 质保书处置
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(qmts0r_pro)

int f_qmts0r_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_order_no = "";
	CString v_heat_no = "";
	CString v_operate = "";
	CString datetime = CDateTime::Now().ToString("yyyy-MM-dd");
	CString slab_cut_time =" ";
	CString slab_cut_time1 = " ";
	CString image = " ";
	CString v_userid = s.userid;

	CModel qmts0r04("TQMTS0R04");

	CDbCommand cmd_inq(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		v_operate = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
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
		/*if (s.userid == "182148"){
			image = "http://10.162.72.16:10004/DiBei/HJ2148.png";
		}
		else if (s.userid == "201805")
		{
			image = "http://10.162.72.16:10004/DiBei/HM1805.png";
		}
		else if (s.userid == "196475")
		{
			image = "http://10.162.72.16:10004/DiBei/HK6475.jpg";
		}
		else if (s.userid == "122406")
		{
			image = "http://10.162.72.16:10004/DiBei/HC2406.jpg";
		}
		else if (s.userid == "196779")
		{
			image = "http://10.162.72.16:10004/DiBei/HK6779.jpg";
		}
		else if (s.userid == "191608")
		{
			image = "http://10.162.72.16:10004/DiBei/HK1608.jpg";
		}
		else if (s.userid == "197927")
		{
			image = "http://10.162.72.16:10004/DiBei/HK7927.jpg";
		}
		else if (s.userid == "205335")
		{
			image = "http://10.162.72.16:10004/DiBei/HM5335.png";
		}
		else{
			image = "http://10.162.72.16:10004/DiBei/MR.png";
		}*/
		if (v_userid == "HJ2148"){
			image = "http://10.162.72.16:10004/DiBei/HJ2148.png";
		}
		else if (v_userid == "HM1805")
		{
			image = "http://10.162.72.16:10004/DiBei/HM1805.png";
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
		else{
			image = "http://10.162.72.16:10004/DiBei/MR.png";
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			qmts0r04.Reset();
			qmts0r04.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_operate == "A")
			{
				/*if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				qmts0r04["AYL_FLAG"] = "1";
				qmts0r04["AYL_MAKE"] = s.userid;
				qmts0r04["AYL_TIME"] = datetime;
				qmts0r04["AYL_IMAGE"] = image;
				if (slab_cut_time !=" "){
					qmts0r04["ANA_TIME"] = slab_cut_time;
				}
				else{
					qmts0r04["ANA_TIME"] = datetime;
				}

				qmts0r04.Update("AYL_FLAG,AYL_MAKE,AYL_TIME,ANA_TIME,AYL_IMAGE","CERTI_PRINT_NO");
			}
			else if (v_operate == "C")
			{
				qmts0r04["CHECK_FLAG"] = "1";
				qmts0r04["CHECK_MAKE"] = s.userid;
				qmts0r04["CHECK_DATE"] = datetime;
				qmts0r04["CHECK_IMAGE"] = image;
				if (slab_cut_time != " "){
					qmts0r04["CHECK_TIME"] = slab_cut_time;
				}
				else{
					qmts0r04["CHECK_TIME"] = datetime;
				}
				qmts0r04.Update("CHECK_FLAG,CHECK_MAKE,CHECK_DATE,CHECK_TIME,CHECK_IMAGE", "CERTI_PRINT_NO");
			}
			else if (v_operate == "D")
			{
				qmts0r04["DECIDE_CODE"] = "1";
				qmts0r04["DECIDER"] = s.userid;
				qmts0r04["JUDGE_TIME"] = datetime;
				qmts0r04["DECIDE_IMAGE"] = image;
				if (slab_cut_time != " "){
					//CDateTime::Now().AddDays(+1).ToString("yyyy-MM-dd");
					qmts0r04["DECIDE_TIME"] = slab_cut_time1;
					Log::Info("", __FUNCTION__, "DECIDE_TIME =[{0}],v_heat_no=[{1}]", slab_cut_time, v_heat_no);
				}
				else{
					qmts0r04["DECIDE_TIME"] = datetime;
				}
				Log::Info("", __FUNCTION__, "DECIDE_TIME =[{0}", slab_cut_time);
				qmts0r04.Update("DECIDE_CODE,DECIDER,JUDGE_TIME,DECIDE_TIME,DECIDE_IMAGE", "CERTI_PRINT_NO");
			}
			else if (v_operate == "U7")
			{
				
				qmts0r04.Update("ANA_TIME", "CERTI_PRINT_NO");
			}
			else if (v_operate == "U8")
			{
				//qmts0r04["CHECK_TIME"] = datetime;
				qmts0r04.Update("CHECK_TIME", "CERTI_PRINT_NO");
			}
			else if (v_operate == "U9")
			{
				//qmts0r04["DECIDE_TIME"] = datetime;
				
				qmts0r04.Update("DECIDE_TIME", "CERTI_PRINT_NO");
			}
			else if (v_operate == "U10")
			{
				//qmts0r04["DECIDE_TIME"] = datetime;

				qmts0r04.Update("DESCRIPTION", "CERTI_PRINT_NO");
			}
			else if (v_operate == "U11")
			{
				qmts0r04.Update("REMARK", "CERTI_PRINT_NO");
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
