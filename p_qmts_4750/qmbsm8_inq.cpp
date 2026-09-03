/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-04-8 15:08:16
Description: 查询成分标准
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(qmbsm8_inq)
int f_qmbsm8_inq(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		sqlstr = " SELECT tqmts0x.ST_NO, TQMTMS0.IDX_NO, TQMTMS0.ELM_NAME, TQMTMS0.ELM_MAIN_MIN, TQMTMS0.ELM_MAIN_MAX, TQMTMS0.ELM_SPE_MIN, TQMTMS0.ELM_SPE_MAX, TQMTS02.ELM_NAME, TQMTS02.MAIN_MIN, TQMTS02.MAIN_MAX, TQMTS02.SPE_MIN, TQMTS02.SPE_MAX  FROM TQMTMS0 LEFT JOIN TQMTS02"
"			ON TQMTMS0.IDX_NO = TQMTS02.IDX_NO"
"			AND TQMTMS0.ELM_CODE = TQMTS02.ELM_CODE"
"			LEFT JOIN tqmts0x ON TQMTMS0.IDX_NO = tqmts0x.ELM_STD_IDX_A"
"			WHERE(TQMTMS0.ELM_MAIN_MIN != TQMTS02.MAIN_MIN"
"			OR TQMTMS0.ELM_MAIN_MAX != TQMTS02.MAIN_MAX)"
"			AND TQMTMS0.IDX_NO IN(SELECT ELM_STD_IDX_A FROM tqmts0x) ";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
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
