/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-16 15:08:16
Description: 化学成分新增
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm_inschemi)


int f_qmtsm_inschemi(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");

	try
	{
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts02.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tqmts02["WHOLE_BACKLOG_CODE"] = bcls_rec->Tables[1].Rows[0]["WHOLE_BACKLOG_CODE"];
			tqmts02["ST_NO"] = bcls_rec->Tables[1].Rows[0]["ST_NO"];
			tqmts02["FACTORY_DIV"] = bcls_rec->Tables[1].Rows[0]["FACTORY_DIV"];
			tqmts02["REC_CREATOR"] = s.userid;
			tqmts02["REC_REVISOR"] = " ";
			tqmts02["REC_CREATE_TIME"] = datetime;
			tqmts02["REC_REVISE_TIME"] = " ";
			tqmts02["VERSION"] = 0;
			tep0002["CODE_CLASS"] = "QMYS";
			tep0002["CODE"] = tqmts02["ELM_CODE"];
			tep0002.Query("CODE_CLASS,CODE");
			tqmts02["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
			tqmts02["ELM_POS"] = tep0002["CODE_DESC_2_CONTENT"];
			tqmts02["ELM_UNIT"] = "%";
			tqmts02.TrimOrBlank();
			tqmts02.Insert();
		}
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


