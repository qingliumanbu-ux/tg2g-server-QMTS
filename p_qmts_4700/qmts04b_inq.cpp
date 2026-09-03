/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-05-11
Description: 查询转炉测厚标准信息
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 查询附件管理
/// <para>
/// 获取输入参数：TQMTS04B(转炉测厚标准信息表) ；
/// </para>
/// <para>数据库表：TQMTS04B(转炉测厚标准信息表));
///
/// 前台QMTS10画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS04B">转炉测厚标准信息    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts04b_inq)


int f_qmts04b_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CDecimal FURNACE_AGE_SM = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts04b("TQMTS04B");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		

		Log::Trace("", "", "qmts04b_inq IN:---FURNACE_AGE_SM = [{0}]", FURNACE_AGE_SM);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"  FROM TQMTS04B "
				"  WHERE 1 = 1 ";
			if (bcls_rec->Tables[0].Columns.Contains("FURNACE_AGE_SM"))
			{
				FURNACE_AGE_SM = bcls_rec->Tables[0].Rows[0]["FURNACE_AGE_SM"].ToDecimal();
				if (FURNACE_AGE_SM!=0)
				{
					sqlstr = sqlstr + " AND FURNACE_AGE <=@FURNACE_AGE_SM AND @FURNACE_AGE_SM <= FURNACE_AGE_MAX";
				}
				
			}
				
			
			sqlstr = sqlstr + "  ORDER BY SEQ_NO ASC ";
			Log::Trace("", "", "qmts04b_inq IN:---sqlstr = [{0}]", (const char*)sqlstr);
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("FURNACE_AGE_SM", FURNACE_AGE_SM);
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
