/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:       
Version:     1.0
Date:        2023-04-4
Description: 查询钢种改判规则表
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 查询混杂元素管理
/// <para>
/// 获取输入参数：TQMTS6C(钢种改判规则表) ；
/// </para>
/// <para>数据库表：TQMTS6C(钢种改判规则表));
///
/// 前台QMTS6C画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS6C">钢种改判规则表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts6c_inq)


int f_qmts6c_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts6c("TQMTS6C");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		tqmts6c.Reset();
		tqmts6c.MergeFrom(bcls_rec->Tables[0].Rows[i]);

		Log::Trace("", "", "qmts6c_inq IN:---HZGL = [{0}]", (const char*)tqmts6c["st_no_plan"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS6C "
				"  WHERE ST_NO_PLAN LIKE @ST_NO_PLAN||'%' "
				"  ORDER BY ST_NO_PLAN ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("ST_NO_PLAN", tqmts6c["ST_NO_PLAN"].ToString().Trim());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
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