/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-11-7 15:08:16
Description: 工艺卡路径（北）
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"


BM2F_ENTERACE_TELE(qm_002111_rcv)

int f_qm_002111_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tqmts0xa("TQMTS0XA");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		for (int i = 0; i < bcls_rec->Tables["tqmts0xa"].Rows.get_Count(); i++)
		{
		    tqmts0xa["PROC_DIV"] = bcls_rec->Tables["tqmts0xa"].Rows[i]["proc_div"].ToString();
			tqmts0xa["ST_NO"] = bcls_rec->Tables["tqmts0xa"].Rows[i]["st_no"].ToString();
			tqmts0xa["FACTORY_DIV"] = bcls_rec->Tables["tqmts0xa"].Rows[i]["factory_div"].ToString();
			tqmts0xa["ST_LINE_NO"] = bcls_rec->Tables["tqmts0xa"].Rows[i]["st_line_no"].ToString();
			tqmts0xa["ST_LINE_DESC"] = bcls_rec->Tables["tqmts0xa"].Rows[i]["st_line_desc"].ToString();
			tqmts0xa["DEFAULT_FLAG"] = bcls_rec->Tables["tqmts0xa"].Rows[i]["default_flag"].ToString();
			tqmts0xa["REMARK"] = bcls_rec->Tables["tqmts0xa"].Rows[i]["remark"].ToString();
			tqmts0xa["REC_CREATOR"] = s.userid;
			tqmts0xa["REC_CREATE_TIME"] = datetime;
			tqmts0xa.Delete("ST_NO,ST_LINE_NO");
			tqmts0xa.Insert();
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


