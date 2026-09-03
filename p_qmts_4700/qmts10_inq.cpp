/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-05-11
Description: 查询附件信息
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 查询附件管理
/// <para>
/// 获取输入参数：TQMTS10(制造标准-附件管理表) ；
/// </para>
/// <para>数据库表：TQMTS10(制造标准-附件管理表));
///
/// 前台QMTS10画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS10">附件管理表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts10_inq)


int f_qmts10_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CString q_st_no = " ";
	CString q_whole_backlog_code = " ";
	CString q_atta_name = " ";
	CString q_factory_div = " ";
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts10("TQMTS10");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		q_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		q_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"].ToString().Trim();
		q_atta_name = bcls_rec->Tables[0].Rows[0]["ATTA_NAME"].ToString().Trim();
		q_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		Log::Trace("", "", "qmts10_inq IN:---ST_NO = [{0}]", (const char*)q_st_no);
		Log::Trace("", "", "qmts10_inq IN:---WHOLE_BACKLOG_CODE = [{0}]", (const char*)q_whole_backlog_code);
		Log::Trace("", "", "qmts10_inq IN:---ATTA_NAME = [{0}]", (const char*)q_atta_name);
		Log::Trace("", "", "qmts10_inq IN:---FACTORY_DIV = [{0}]", (const char*)q_factory_div);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"  FROM TQMTS10 "
				"  WHERE 1 = 1 ";
			if (q_st_no.Trim() != "")
			{
				sqlstr = sqlstr + "  AND SEQ_NO IN (SELECT SEQ_NO FROM TQMTS10X WHERE ST_NO LIKE '%'||@q_st_no||'%') ";
				cmd_inq.Parameters.Set("q_st_no", q_st_no.Trim());
			}
			if (q_whole_backlog_code.Trim() != "")
			{
				sqlstr = sqlstr + "  AND WHOLE_BACKLOG_CODE = @q_whole_backlog_code ";
				cmd_inq.Parameters.Set("q_whole_backlog_code", q_whole_backlog_code.Trim());
			}
			if (q_atta_name.Trim() != "")
			{
				sqlstr = sqlstr + "  AND ATTA_NAME LIKE '%'||@q_atta_name||'%' ";
				cmd_inq.Parameters.Set("q_atta_name", q_atta_name.Trim());
			}
			if (q_factory_div.Trim() != "")
			{
				sqlstr = sqlstr + "  AND FACTORY_DIV = @q_factory_div ";
				cmd_inq.Parameters.Set("q_factory_div", q_factory_div.Trim());
			}
			sqlstr = sqlstr + "  ORDER BY SEQ_NO ASC ";
			Log::Trace("", "", "qmts10_inq IN:---sqlstr = [{0}]", (const char*)sqlstr);
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetMsg() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
