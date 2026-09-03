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
/// 前台QMTS10画面的gridview1点击调用    </para>
/// </summary>
/// <param name="TQMTS10">附件管理表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts10_inq1)


int f_qmts10_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CDecimal q_seq_no = 0;
	CString q_factory_div = " ";
	CString q_atta_name = " ";
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts10("TQMTS10");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		q_seq_no = bcls_rec->Tables[0].Rows[0]["SEQ_NO"].ToDecimal();
		q_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();

		Log::Trace("", "", "qmts10_inq IN:---SEQ_NO = [{0}]", q_seq_no);
		Log::Trace("", "", "qmts10_inq IN:---WHOLE_BACKLOG_CODE = [{0}]", (const char*)q_factory_div);

		//已对应出钢记号
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT ST_NO "
				"  FROM TQMTS10X "
				"  WHERE 1 = 1 ";
			if (q_seq_no != 0)
			{
				sqlstr = sqlstr + "  AND SEQ_NO = @q_seq_no ";
				cmd_inq.Parameters.Set("q_seq_no", q_seq_no);
			}
			
			sqlstr = sqlstr + "  ORDER BY ST_NO ASC ";
			Log::Trace("", "", "qmts10_inq1 IN111:---sqlstr = [{0}]", (const char*)sqlstr);
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();


		//未对应出钢记号
		bcls_ret->Tables.Add();
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT ST_NO "
				"  FROM TQMTS0X "
				"  WHERE 1 = 1 ";
			if (q_seq_no != 0)
			{
				sqlstr = sqlstr + "  AND ST_NO NOT IN (SELECT ST_NO FROM TQMTS10X WHERE SEQ_NO = @q_seq_no ) ";
				cmd_inq.Parameters.Set("q_seq_no", q_seq_no);
			}

			if (q_factory_div.Trim() != "")
			{
				sqlstr = sqlstr + "  AND FACTORY_DIV = @q_factory_div ";
				cmd_inq.Parameters.Set("q_factory_div", q_factory_div);
			}

			sqlstr = sqlstr + "  ORDER BY ST_NO ASC ";
			Log::Trace("", "", "qmts10_inq1 IN222:---sqlstr = [{0}]", (const char*)sqlstr);
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
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
