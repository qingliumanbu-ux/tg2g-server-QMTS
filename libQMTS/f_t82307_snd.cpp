/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: 炼钢二厂转炉冶炼操作记录
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t82307_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_time(conn);
	CDbCommand cmd_02(conn);
	CString time = "";
	CModel tmmsm21("TMMSM21");
	CString wt = "";
	try
	{
		CString epex_number = "T82307";
		CString deal_flag = bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		CString heat_no = bcls_rec->Tables["T823"].Rows[0]["HEAT_NO"].ToString();
		tmmsm21["SM_PLAN_NOL2"] = bcls_rec->Tables["T823"].Rows[0]["SM_PLAN_NOL2"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		tmmsm21["HEAT_NO"] = heat_no;
		tmmsm21.Query("HEAT_NO,SM_PLAN_NOL2");
		Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", tmmsm21["HEAT_NO"].ToString());
		tmmsm21.TrimOrBlank();
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//炉号
		if (epex.SetValue("HEAT_NO", 0, tmmsm21["HEAT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//日期
		if (epex.SetValue("DATE_TIME", 0, tmmsm21["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//作业计划号
		if (epex.SetValue("PLAN_NO", 0, tmmsm21["L2_PROC_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//Grade钢牌号
		if (epex.SetValue("STEELGRADE", 0, tmmsm21["ST_NO"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}
		//SMP_MODE
		if (epex.SetValue("SMP_MODE", 0, tmmsm21["SMP_MODE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//班次
		if (epex.SetValue("F_CLASS", 0, tmmsm21["PROD_SHIFT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//班组
		if (epex.SetValue("GROUP_NO", 0, tmmsm21["PROD_SHIFT_GROUP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//溅渣开始
		if (epex.SetValue("PROC_START_T", 0, tmmsm21["SLAG_START_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//溅渣N2
		/*if (epex.SetValue("FLUX_N2", 0, tmmsm21[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//溅渣结束时刻
		if (epex.SetValue("PROC_END_T", 0, tmmsm21["SLAG_END_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		// 铁水罐号
		if (epex.SetValue("IRON_LADLE_NO", 0, tmmsm21["IRON_LADLE_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//HM
		/*if (epex.SetValue("TIME_STAMP", 0, tmmsm21[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//开始吹炼时刻
		if (epex.SetValue("BUCKET_START_TIME", 0, tmmsm21["BLOW_START_TIME1"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//吹炼结束时刻
		if (epex.SetValue("BUCKET_END_TIME", 0, tmmsm21["BLOW_END_TIME1"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//Tap to Tap
		/*if (epex.SetValue("TIME", 0, tmmsm21[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//操作者
		if (epex.SetValue("OPERATOR", 0, tmmsm21["ASSISTANT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//C
		if (epex.SetValue("C", 0, tmmsm21["IRON_C"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//SI
		if (epex.SetValue("SI", 0, tmmsm21["IRON_SI"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//MN
		if (epex.SetValue("MN", 0, tmmsm21["IRON_MN"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//P
		if (epex.SetValue("P", 0, tmmsm21["IRON_P"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//S
		if (epex.SetValue("S", 0, tmmsm21["IRON_S"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//V
		/*if (epex.SetValue("V", 0, tmmsm21["IRON_"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//TI
		/*if (epex.SetValue("TI", 0, tmmsm21[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//温度
		/*if (epex.SetValue("TEMP", 0, tmmsm21["IRON_TEMP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//重量
		if (epex.SetValue("WEIGHT", 0, tmmsm21["ACTRESULT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//回炉重量
		/*if (epex.SetValue("HL_WT", 0, tmmsm21[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//外购碳素废钢
		if (epex.SendTele() < 0)
		{
			Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		epex.Uninitialize();
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


