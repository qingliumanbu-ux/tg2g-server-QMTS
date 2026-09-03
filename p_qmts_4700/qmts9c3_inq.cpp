/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      李翔宇
Version:     1.0
Date:        2023-05-06
Description: 查询压下基准管理
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 查询压下基准管理
/// <para>
/// 获取输入参数：TQMTS9C3(压下基准管理表) ；
/// </para>
/// <para>数据库表：TQMTS9C3(压下基准管理表));
///
/// 前台QMTS9C3画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS9C3">压下基准管理表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts9c3_inq)


int f_qmts9c3_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CString Q_PATTERN_NO = " ";
	CString Q_STEEL_GROUP = " ";
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts9c3("TQMTS9C3");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//tqmts9c3.Reset();
		//tqmts9c3.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//Q_PATTERN_NO = (const char*)tqmts9c3["PATTERN_NO"].ToString();
		//Q_STEEL_GROUP = (const char*)tqmts9c3["STEEL_GROUP"].ToString();
		Q_PATTERN_NO = bcls_rec->Tables[0].Rows[0]["PATTERN_NO"].ToString().Trim();
		Q_STEEL_GROUP = bcls_rec->Tables[0].Rows[0]["STEEL_GROUP"].ToString().Trim();

		Log::Trace("", "", "qmts9c3_inq IN:---Q_PATTERN_NO = [{0}]", (const char*)Q_PATTERN_NO);
		Log::Trace("", "", "qmts9c3_inq IN:---Q_STEEL_GROUP = [{0}]", (const char*)Q_STEEL_GROUP);

		//Log::Trace("", "", "qmts9c3_inq IN:---Q_PATTERN_NO = [{0}]", (const char*)tqmts9c3["PATTERN_NO"].ToString());
		//Log::Trace("", "", "qmts9c3_inq IN:---Q_STEEL_GROUP = [{0}]", (const char*)tqmts9c3["STEEL_GROUP"].ToString());
		//Log::Trace("", "", "qmts9c3_inq IN:---PATTERN_NO,STEEL_GROUP = [{0},{1}]", (const char*)tqmts9c3["PATTERN_NO"].ToString(), (const char*)tqmts9c3["STEEL_GROUP"].ToString());


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"  FROM TQMTS9C3 "
				"  WHERE 1 = 1 ";
			if (Q_PATTERN_NO.Trim() != "")
				sqlstr = sqlstr + "  AND PATTERN_NO LIKE '%'||@Q_PATTERN_NO||'%' ";
			if (Q_STEEL_GROUP.Trim() != "")
				sqlstr = sqlstr + "  AND STEEL_GROUP LIKE '%'||@Q_STEEL_GROUP||'%' ";
			sqlstr = sqlstr + "  ORDER BY PATTERN_NO ASC ";
			Log::Trace("", "", "qmts9c3_inq IN:---sqlstr = [{0}]", (const char*)sqlstr);
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("Q_PATTERN_NO", Q_PATTERN_NO.Trim());
		cmd_inq.Parameters.Set("Q_STEEL_GROUP", Q_STEEL_GROUP.Trim());
		//cmd_inq.Parameters.Set("PATTERN_NO", tqmts9c3["PATTERN_NO"].ToString().Trim());
		//cmd_inq.Parameters.Set("STEEL_GROUP", tqmts9c3["STEEL_GROUP"].ToString().Trim());
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
