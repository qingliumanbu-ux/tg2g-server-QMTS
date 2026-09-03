/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-11-23 15:08:16
Description: 代表成分取消
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmts25s_rep_unsel)
int f_qmts_rep_unsel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//代表成分取消
int f_qmts25s_rep_unsel(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		EIClass elm_cal;
		elm_cal.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		elm_cal.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
		elm_cal.Tables[0].Rows.Add();
		elm_cal.Tables[0].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		elm_cal.Tables[0].Rows[0]["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString();
		doFlag = f_qmts_rep_unsel(&elm_cal, bcls_ret, conn);
		if (doFlag < 0)
		{
			doFlag = 0;
			s.flag = 0;
			sprintf(s.msg, "");
			throw CApplicationException(-1, s.msg, log.Location);
		}
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
