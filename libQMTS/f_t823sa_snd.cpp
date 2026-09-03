/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: 连铸难度系数实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t823sa_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);
	EPEX epex;
	CModel tmmsm31("TMMSM31");
	CModel tmmsmt823sj("TMMSMT823SJ");
	CString ljls = " ";
	CString cs14 = " ";
	try
	{ 
		CString epex_number = "T823SA";
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获取熔炼号
		tmmsm31["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tmmsm31["L2_PROC_NO"] = bcls_rec->Tables[0].Rows[0]["L2_PROC_NO"].ToString().Trim();
		tmmsm31.Query("HEAT_NO,L2_PROC_NO");
		tmmsm31.TrimOrBlank();
		CString deal_flag = "I";
		tmmsmt823sj["DEV_CODE"] = tmmsm31["DEV_CODE"].ToString();
		//操作标记
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//生产日期
		if (epex.SetValue("PROD_DATE", 0, tmmsm31["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["PROD_DATE"] = tmmsm31["PROD_DATE"].ToString();
		//成本中心
		/*if (epex.SetValue("COST_CENTER", 0, tmmsm31[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//炼钢记号
		if (epex.SetValue("ST_NO", 0, tmmsm31["ST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["ST_NO"] = tmmsm31["ST_NO"].ToString();
		//炉号
		if (epex.SetValue("HEAT_NO", 0, tmmsm31["HEAT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsmt823sj["HEAT_NO"] = tmmsm31["HEAT_NO"].ToString();
		//处理量
		cmd_inq.SetCommandText(" SELECT SUM(RECEIVE_WEIGHT) FROM VMMSM01  where HEAT_NO='" + tmmsm31["HEAT_NO"].ToString() + "' ");
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
		/*if (epex.SetValue("CS15", 0, tmmsm31[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		/*cmd.SetCommandText(" select count(*) "
			" from TMMSMGY05 "
			" where(TD_NO_1, CAST_DIV_NO, CAST_DIV_NO_1) IN( "
			" SELECT TD_NO_1, CAST_DIV_NO, CAST_DIV_NO_1 "
			" FROM TMMSMGY05 "
			" WHERE HEAT_NO = '" + tmmsm31["HEAT_NO"].ToString() + "') ");  */
		Log::Trace("", "", "gz[{0}]", tmmsm31["ST_NO"].ToString().Substring(0, 1));
		if (tmmsm31["ST_NO"].ToString().Substring(0, 1) == '2' || tmmsm31["ST_NO"].ToString().Substring(0, 1) == '3')
		{
			cmd.SetCommandText(" SELECT COUNT(*) OVER(PARTITION BY CAST_DIV_NO, TD_NO_1, CAST_DIV_NO_1) AS LJLS FROM "
				" (SELECT T1.END_TIME, T1.HEAT_NO, T1.ST_NO, T2.CAST_DIV_NO, T2.TD_NO_1, T2.CAST_DIV_NO_1 "
				" FROM TMMSM31 T1 "
				" LEFT JOIN TMMSMGY05 T2 ON T1.HEAT_NO = T2.HEAT_NO WHERE SUBSTR(T1.ST_NO, 0, 1) = '2'  OR SUBSTR(T1.ST_NO, 0, 1) = '3')T3 "
				" LEFT JOIN "
				" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
				" GROUP BY HEAT_NO)T4 "
				" ON T3.HEAT_NO = T4.HEAT_NO WHERE T3.HEAT_NO = '" + tmmsm31["HEAT_NO"].ToString() + "' ");
		}
		if (tmmsm31["ST_NO"].ToString().Substring(0, 1) == '1')
		{
			cmd.SetCommandText(" SELECT COUNT(*) OVER(PARTITION BY DEV_CODE, CAST_DIV_NO, TD_NO_1 ORDER BY DEV_CODE, CAST_DIV_NO, LADLE_LEAVE_TIME, TD_NO_1) AS LJLS "
				" FROM(SELECT * FROM TMMSM31 WHERE SUBSTR(ST_NO, 0, 1) = '1') T1 "
				" LEFT JOIN "
				" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
				" GROUP BY HEAT_NO)T2 "
				" ON T1.HEAT_NO = T2.HEAT_NO WHERE T1.HEAT_NO = '" + tmmsm31["HEAT_NO"].ToString() + "'");
		}
		cmd.ExecuteReader();
		if (cmd.Read()){
			ljls = cmd.GetString(1);
		}
		cmd.Close();
		//连浇炉数
		if (epex.SetValue("CS16", 0, ljls) < 0)
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
		tmmsmt823sj["CS16"] = ljls;
		tmmsmt823sj["TC_NO"] = "T823SA";
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


