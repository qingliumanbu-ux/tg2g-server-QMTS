/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: 脱硫站难度系数实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t823s2_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CModel tmmsm14("TMMSMKR14");
	CModel tmmsmt823sj("TMMSMT823SJ");
	 
	try
	{
		//CString epex_number = "T823S2";
		//if (epex.Initialize(epex_number) < 0)
		//{
		//	sprintf(s.msg, (const char*)epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		////获取计划号
		//tmmsm14["DES_ID"] = bcls_rec->Tables["T823S"].Rows[0]["DES_ID"].ToString().Trim();
		//Log::Info("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm14["DES_ID"].ToString());
		//tmmsm14.Query("DES_ID");
		//tmmsm14.TrimOrBlank();
		//tmmsmt823sj["DEV_CODE"] = tmmsm14["DES_STATION_NO"].ToString();
		//CString deal_flag = bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		////操作标记
		//if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		//{
		//	sprintf(s.msg, (const char*)epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		////生产日期
		//if (epex.SetValue("PROD_DATE", 0, tmmsm14["DES_START"].ToString().SubstringNE(0,8)) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//tmmsmt823sj["PROD_DATE"] = tmmsm14["DES_START"].ToString().SubstringNE(0, 8);
		//Log::Info("", __FUNCTION__, "DES_START=[{0}]", tmmsm14["DES_START"].ToString().SubstringNE(0, 8));
		////成本中心
		///*if (epex.SetValue("COST_CENTER", 0, tmmsm14[""].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}*/

		////炼钢记号
		//if (epex.SetValue("ST_NO", 0, tmmsm14["STEEL_GRADE"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//tmmsmt823sj["ST_NO"] = tmmsm14["STEEL_GRADE"].ToString();
		////炉号
		//if (epex.SetValue("HEAT_NO", 0, tmmsm14["HEAT_NO"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		////冶炼时间
		///*if (epex.SetValue("MELT_DURATION", 0, tmmsm14[""].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}*/

		////搅拌时间
		//if (epex.SetValue("SPRAY_DURATION", 0, tmmsm14["STIRRER_DURATION"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//tmmsmt823sj["SPRAY_DURATION"] = tmmsm14["STIRRER_DURATION"].ToString();
		//Log::Info("", __FUNCTION__, "SPRAY_DURATION=[{0}]", tmmsm14["STIRRER_DURATION"].ToString());
		////脱硫剂
		///*if (epex.SetValue("CS12", 0, tmmsm14[""].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}*/
		//tmmsmt823sj["TC_NO"] = "T823S2";
		//tmmsmt823sj["REC_CREATE_TIME"] = datetime;
		//tmmsmt823sj["DATI_MSG_SENT"] = datetime;
		//tmmsmt823sj["SEND_FLAG"] = "1";
		//tmmsmt823sj["DEAL_FLAG"] = deal_flag;
		//tmmsmt823sj.Insert();
		//if (epex.SendTele() < 0)
		//{
		//	Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
		//	sprintf(s.msg, (const char*)epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//epex.Uninitialize();
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


