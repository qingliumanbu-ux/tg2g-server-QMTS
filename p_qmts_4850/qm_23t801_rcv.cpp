/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-12-11 15:08:16
Description: 二炼钢炉次改判结果电文
**************************************************/

#include "stdafx.h"
#include "epex.h"
BM2F_ENTERACE_TELE(qm_23t801_rcv)

int f_qm_23t801_rcv(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmtst801("TQMTST801");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
	
		tqmtst801["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		tqmtst801["ST_NO"] = bcls_rec->Tables[0].Rows[0]["HTNO"].ToString();
		tqmtst801["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
		tqmtst801["CHANGE_ST_NO"] = bcls_rec->Tables[0].Rows[0]["FIN_JDG_ST_NO"].ToString();
		tqmtst801["CHG_ST_NO_FLAG"] = bcls_rec->Tables[0].Rows[0]["GG_FLAG"].ToString();
		tqmtst801["RESP"] = bcls_rec->Tables[0].Rows[0]["PERSON_IN_CHARGE"].ToString();
		tqmtst801["DATI_MSG_SENT"] = bcls_rec->Tables[0].Rows[0]["SEND_TIME"].ToString();
		tqmtst801["REMARK1"] = bcls_rec->Tables[0].Rows[0]["remark1"].ToString();
		tqmtst801["REMARK2"] = bcls_rec->Tables[0].Rows[0]["remark2"].ToString();
		tqmtst801["REMARK3"] = bcls_rec->Tables[0].Rows[0]["remark3"].ToString();
		tqmtst801["REMARK4"] = bcls_rec->Tables[0].Rows[0]["remark4"].ToString();
		tqmtst801["REMARK5"] = bcls_rec->Tables[0].Rows[0]["remark5"].ToString();
		tqmtst801["REMARK6"] = bcls_rec->Tables[0].Rows[0]["remark6"].ToString();
		tqmtst801["REMARK7"] = bcls_rec->Tables[0].Rows[0]["remark7"].ToString();
		tqmtst801["REMARK_T8"] = bcls_rec->Tables[0].Rows[0]["remark8"];
		tqmtst801["REMARK_T9"] = bcls_rec->Tables[0].Rows[0]["remark9"];
		tqmtst801["REMARK_T10"] = bcls_rec->Tables[0].Rows[0]["remark10"];
		tqmtst801.TrimOrBlank();
		tqmtst801.Insert();
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
