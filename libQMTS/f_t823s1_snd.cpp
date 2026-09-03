/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: 三脱预处理难度系数实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t823s1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel tmmsm14("TMMSM14");
	CModel tmmsmt823sj("TMMSMT823SJ");
	try
	{
		//初始化
		//CString epex_number = "T823S1";
		//if (epex.Initialize(epex_number) < 0) 
		//{
		//	sprintf(s.msg, (const char*)epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//CString deal_flag = bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		////获取熔炼号
		//tmmsm14["HEAT_NO"] = bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"].ToString().Trim();
		////获取计划号
		//tmmsm14["L2_PROC_NO"] = bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"].ToString().Trim();
		//tmmsm14.Query("L2_PROC_NO");
		//tmmsm14.TrimOrBlank();
		//tmmsmt823sj["DEV_CODE"] = tmmsm14["DEV_CODE"].ToString();
		////操作标记
		//if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		//{
		//	sprintf(s.msg, (const char*)epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		////生产日期
		//if (epex.SetValue("PROD_DATE", 0, tmmsm14["PROD_DATE"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//tmmsmt823sj["PROD_DATE"] = tmmsm14["PROD_DATE"].ToString();
		////成本中心
		///*if (epex.SetValue("COST_CENTER", 0, tmmsm14[""].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}*/


		////炉号
		//if (epex.SetValue("HEAT_NO", 0, tmmsm14["HEAT_NO"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//tmmsmt823sj["HEAT_NO"] = tmmsm14["HEAT_NO"].ToString();
		////炼钢记号
		//if (epex.SetValue("ST_NO", 0, tmmsm14["ST_NO"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//tmmsmt823sj["ST_NO"] = tmmsm14["ST_NO"].ToString();
		////处理前铁水量
		//if (epex.SetValue("CS07", 0, tmmsm14["PREV_WT"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//tmmsmt823sj["CS07"] = tmmsm14["PREV_WT"].ToString();
		////处理后铁水量
		//if (epex.SetValue("CS08", 0, tmmsm14["FIN_WT"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//tmmsmt823sj["CS08"] = tmmsm14["FIN_WT"].ToString();
		////氧气
		///*if (epex.SetValue("CS02", 0, tmmsm14[""].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}*/

		////氮气
		///*if (epex.SetValue("CS03", 0, tmmsm14[""].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}*/

		////红泥球
		///*if (epex.SetValue("CS09", 0, tmmsm14[""].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}*/

		////脱磷剂
		///*if (epex.SetValue("CS10", 0, tmmsm14[""].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}*/
		//tmmsmt823sj["TC_NO"] = "T823S1";
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


