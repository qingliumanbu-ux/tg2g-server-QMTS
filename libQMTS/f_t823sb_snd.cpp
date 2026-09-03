/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: 修磨难度系数实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t823sb_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);
	CModel tmmsm34("TMMSM34");
	CModel tmmsmt823sj("TMMSMT823SJ");
	CString mend_before_weight = " ";
	CString mend_after_weight = " ";
	try
	{
		CString epex_number = "T823SB";
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		tmmsm34["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tmmsm34["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		tmmsm34.Query("HEAT_NO,MAT_NO");
		tmmsm34.TrimOrBlank();
		CString deal_flag = "I";
		tmmsmt823sj["DEV_CODE"] = tmmsm34["DEV_CODE"].ToString();
		//操作标记
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//生产日期
		if (epex.SetValue("PROD_DATE", 0, tmmsm34["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["PROD_DATE"] = tmmsm34["PROD_DATE"].ToString();
		//成本中心
		/*if (epex.SetValue("COST_CENTER", 0, tmmsm31[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//炼钢记号
		if (epex.SetValue("ST_NO", 0, tmmsm34["ST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["ST_NO"] = tmmsm34["ST_NO"].ToString();
		//炉号
		if (epex.SetValue("HEAT_NO", 0, tmmsm34["HEAT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["HEAT_NO"] = tmmsm34["HEAT_NO"].ToString();
		/*//处理量
		cmd_inq.SetCommandText(" SELECT SUM(RECEIVE_WEIGHT) FROM VMMSM01  where HEAT_NO='" + tmmsm34["HEAT_NO"].ToString() + "' ");
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
		tmmsmt823sj["CS14"] = cs14;*/

		cmd_inq.SetCommandText(" SELECT * FROM ( "
			" SELECT MEND_BEFORE_WEIGHT, MEND_AFTER_WEIGHT, HEAT_NO FROM( "
			" SELECT T3.*, ROW_NUMBER() OVER(PARTITION BY HEAT_NO ORDER BY END_TIME) AS RN FROM( "
			" SELECT  GRINDING_START_TIME AS END_TIME, T.HEAT_NO, T2.MEND_BEFORE_WEIGHT, T2.MEND_AFTER_WEIGHT "
			" FROM(SELECT GRINDING_START_TIME, HEAT_NO, ST_NO FROM TMMSM34 WHERE 1 = 1) T "
			" LEFT JOIN(SELECT T1.HEAT_NO, sum(mend_before_weight) as mend_before_weight, "
			" sum(mend_after_weight) as mend_after_weight FROM TMMSM34 T1  WHERE 1 = 1 "
			" GROUP BY T1.HEAT_NO)T2  ON T.HEAT_NO = T2.HEAT_NO "
			" ORDER BY GRINDING_START_TIME)T3) WHERE RN = '1' ORDER BY END_TIME)  where HEAT_NO = '" + tmmsm34["HEAT_NO"].ToString() + "' ");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read()){
			mend_before_weight = cmd_inq.GetString(1);
			mend_after_weight = cmd_inq.GetString(2);
		}
		cmd_inq.Close();
		if (mend_before_weight == " " || mend_before_weight == ""){
			mend_before_weight = "0";
		}
		if (epex.SetValue("mend_before_weight", 0, mend_before_weight) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
		}
		tmmsmt823sj["MEND_BEFORE_WEIGHT"] = mend_before_weight; 

		if (mend_after_weight == " " || mend_after_weight == ""){
			mend_after_weight = "0";
		}
		if (epex.SetValue("mend_after_weight", 0, mend_after_weight) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
		}
		tmmsmt823sj["MEND_AFTER_WEIGHT"] = mend_after_weight;

		/*//磨前重量
		if (epex.SetValue("MEND_BEFORE_WEIGHT", 0, tmmsm34["MEND_BEFORE_WEIGHT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"].ToString();

		//磨后重量
		if (epex.SetValue("MEND_AFTER_WEIGHT", 0, tmmsm34["MEND_AFTER_WEIGHT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["MEND_AFTER_WEIGHT"] = tmmsm34["MEND_AFTER_WEIGHT"].ToString();*/

		if (tmmsmt823sj.QueryCount("HEAT_NO,DEV_CODE") > 1)
		{
			deal_flag = "U";
		}
		else
		{
			deal_flag = "I";
		}
		tmmsmt823sj["TC_NO"] = "T823SB";
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


