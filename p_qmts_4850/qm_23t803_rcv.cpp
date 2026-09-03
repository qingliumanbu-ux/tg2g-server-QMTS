/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-12-11 15:08:16
Description: 二炼钢异常坯处置意见
**************************************************/

#include "stdafx.h"
#include "epex.h"
BM2F_ENTERACE_TELE(qm_23t803_rcv)

int f_qm_23t803_rcv(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmtst803("TQMTST803");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		tqmtst803["SLAB_NO"] = bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString();
		tqmtst803["ST_CLASS"] = bcls_rec->Tables[0].Rows[0]["LG_ST"].ToString();
		tqmtst803["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		tqmtst803["CUT_TIME"] = bcls_rec->Tables[0].Rows[0]["CUT_TIME"].ToString();
		tqmtst803["CHANGE_ST_NO"] = bcls_rec->Tables[0].Rows[0]["FIN_JDG_LG_ST"].ToString();
		tqmtst803["DEAL_NOTION"] = bcls_rec->Tables[0].Rows[0]["DISPOSAL_OPINION"].ToString();
		tqmtst803["RESP"] = bcls_rec->Tables[0].Rows[0]["PERSON_IN_CHARGE"].ToString();
		tqmtst803["SEND_TIME"] = bcls_rec->Tables[0].Rows[0]["SEND_TIME"].ToString();
		tqmtst803["REMARK1"] = bcls_rec->Tables[0].Rows[0]["remark1"].ToString();
		tqmtst803["REMARK2"] = bcls_rec->Tables[0].Rows[0]["remark2"].ToString();
		tqmtst803["REMARK3"] = bcls_rec->Tables[0].Rows[0]["remark3"].ToString();
		tqmtst803["REMARK4"] = bcls_rec->Tables[0].Rows[0]["remark4"].ToString();
		tqmtst803["REMARK5"] = bcls_rec->Tables[0].Rows[0]["remark5"].ToString();
		tqmtst803["REMARK6"] = bcls_rec->Tables[0].Rows[0]["remark6"].ToString();
		tqmtst803["REMARK7"] = bcls_rec->Tables[0].Rows[0]["remark7"].ToString();
		tqmtst803["REMARK_T8"] = bcls_rec->Tables[0].Rows[0]["remark8"];
		tqmtst803["REMARK_T9"] = bcls_rec->Tables[0].Rows[0]["remark9"];
		tqmtst803["REMARK_T10"] = bcls_rec->Tables[0].Rows[0]["remark10"];
		tqmtst803.TrimOrBlank();
		tqmtst803.Insert();
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
