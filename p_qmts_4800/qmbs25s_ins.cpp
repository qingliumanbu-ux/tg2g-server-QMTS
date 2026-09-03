/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-10-01 14:48:51
Description: 工序成分画面新增
**************************************************/

#include "stdafx.h"
BM2F_ENTERACE(qmbs25s_ins)

int f_qmts_24_ins_form(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//试样主表TQMTS24表新增
int f_qmts_25_ins_form(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//试样子表TQMTS25表新增
int f_qmts_elm_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//成分判定
int f_qmts_sample_init(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//调用试样号生成
int f_qmbs25s_ins(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd(conn);
	try
	{
		
		//调用试样号生成
		doFlag = f_qmts_sample_init(bcls_rec, bcls_ret, conn);
		Log::Trace("", "", "st_sample_no = {0}", bcls_rec->Tables[0].Rows[0]["st_sample_no"].ToString());
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/* 获取传入的表名 */
		//调用试样成分新增函数，新增TQMTS24表
		doFlag = f_qmts_24_ins_form(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//调用试样成分新增函数，新增TQMTS25表
		doFlag = f_qmts_25_ins_form(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//成分判定
		doFlag = f_qmts_elm_jud(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
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
