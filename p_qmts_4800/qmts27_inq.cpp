/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      wsl
Version:     1.0
Date:        2024-03-13
Description: 低倍判断
**************************************************/


//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中
// service入口
BM2F_ENTERACE(qmts27_inq)


int f_qmts27_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString MAT_NO = "";
	CString sqlstr = "";
	CString mat_no1 = "";

	/* 实体类定义 */
	CModel tqmts27("TQMTS27");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/*获得传入参数*/
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		}

		MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		Log::Trace("", "", "qmts27_inq IN:---MAT_NO = [{0}]", MAT_NO);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT MAT_NO FROM TQMTS27 WHERE 1=1";

			if (MAT_NO.Trim() != "")
				sqlstr += " AND  MAT_NO= @MAT_NO";
			break;
		}
		Log::Trace("", "", "qmts27_inq IN:---sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NO", MAT_NO.Trim());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			mat_no1 = cmd_inq.GetString(1);
		}

		cmd_inq.Close();
		if (mat_no1 != "")
		{
			sprintf(s.msg, "该物料信息已新增！！！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
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
