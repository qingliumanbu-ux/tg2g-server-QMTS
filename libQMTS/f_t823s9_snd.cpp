/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: VOD难度系数实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t823s9_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CModel tmmsmt823sj("TMMSMT823SJ");
	CModel tmmsm25("TMMSM25");
	CDbCommand cmd_inq(conn);
	CString cs14 = " ";

	try
	{
		CString epex_number = "T823S9";
		if (epex.Initialize(epex_number) < 0) 
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获取熔炼号
		tmmsm25["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tmmsm25["PROC_NO"] = bcls_rec->Tables[0].Rows[0]["PROC_NO"].ToString().Trim();
		tmmsm25["L2_PROC_NO"] = bcls_rec->Tables[0].Rows[0]["L2_PROC_NO"].ToString().Trim();
		tmmsm25.Query("HEAT_NO,L2_PROC_NO");
		tmmsm25.TrimOrBlank();
		CString deal_flag = "I";
		tmmsmt823sj["DEV_CODE"] = tmmsm25["DEV_CODE"].ToString();
		//操作标记
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//生产日期
		if (epex.SetValue("PROD_DATE", 0, tmmsm25["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["PROD_DATE"] = tmmsm25["PROD_DATE"].ToString();
		//成本中心
		/*if (epex.SetValue("COST_CENTER", 0, tmmsm25[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//炼钢记号
		if (epex.SetValue("ST_NO", 0, tmmsm25["ST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["ST_NO"] = tmmsm25["ST_NO"].ToString();
		//炉号
		if (epex.SetValue("HEAT_NO", 0, tmmsm25["HEAT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["HEAT_NO"] = tmmsm25["HEAT_NO"].ToString();
		//处理量
		cmd_inq.SetCommandText(" SELECT SUM(RECEIVE_WEIGHT) FROM VMMSM01  where HEAT_NO='" + tmmsm25["HEAT_NO"].ToString() + "' ");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read()){
			cs14 = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		if (cs14 == " " || cs14 == ""){
			cs14 = "0";
		}
		if (epex.SetValue("CS14", 0, cs14) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
		}
		tmmsmt823sj["CS14"] = cs14;
		//炉数
		/*if (epex.SetValue("CS15", 0, tmmsm25[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//冶炼时间
		if (tmmsm25["END_TIME"].ToString().Trim() == " "
			|| tmmsm25["START_TIME"].ToString().Trim() == " "
			|| tmmsm25["END_TIME"].ToString().Trim() == ""
			|| tmmsm25["START_TIME"].ToString().Trim() == "")
		{
			tmmsmt823sj["MELT_DURATION"] = 0;
		}
		else{
			if (epex.SetValue("MELT_DURATION", 0, (CDateTime::Parse(tmmsm25["END_TIME"]) - CDateTime::Parse(tmmsm25["START_TIME"])).TotalMinutes()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//tmmsmt823sj["MELT_DURATION"] = tmmsm25["MELT_DURATION"].ToString();
			tmmsmt823sj["MELT_DURATION"] = (CDateTime::Parse(tmmsm25["END_TIME"]) - CDateTime::Parse(tmmsm25["START_TIME"])).TotalMinutes();
		}

		//氧气
		if (epex.SetValue("CS02", 0, tmmsm25["OXYGEN_FINAL"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["CS02"] = tmmsm25["OXYGEN_FINAL"].ToString();
		//氮气
		if (epex.SetValue("CS03", 0, tmmsm25["EAF_TOTAL_N2_CONS"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["CS03"] = tmmsm25["EAF_TOTAL_N2_CONS"].ToString();
		//氩气
		if (epex.SetValue("CS04", 0, tmmsm25["AR_SUM_COMSUME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsmt823sj.QueryCount("HEAT_NO,DEV_CODE") > 1)
		{
			deal_flag = "U";
		}
		else
		{
			deal_flag = "I";
		}
		tmmsmt823sj["CS04"] = tmmsm25["AR_SUM_COMSUME"].ToString();
		tmmsmt823sj["TC_NO"] = "T823S9";
		tmmsmt823sj["REC_CREATE_TIME"] = datetime;
		tmmsmt823sj["DATI_MSG_SENT"] = datetime;
		tmmsmt823sj["SEND_FLAG"] = "1";
		tmmsmt823sj["DEAL_FLAG"] = deal_flag;
		tmmsmt823sj.Insert();
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


