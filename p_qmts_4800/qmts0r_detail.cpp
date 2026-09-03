/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   bhy
Version:    1.0
Date:     2024-08-26 9:13:56
Description: 质保书查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(qmts0r_detail)

int f_qmts0r_detail(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_certi_print_no = "";
	CString v_certi_bill_no = "";



	CDbCommand cmd_inq(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("CERTI_PRINT_NO"))
			v_certi_print_no = bcls_rec->Tables[0].Rows[0]["CERTI_PRINT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CERTI_BILL_NO"))
			v_certi_bill_no = bcls_rec->Tables[0].Rows[0]["CERTI_BILL_NO"].ToString();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "v_certi_print_no =[{0}]", v_certi_print_no);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = " SELECT * FROM QMTS0R WHERE 1 = 1 ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}

			break;
		}
		cmd_inq.Parameters.Set("v_certi_print_no", v_certi_print_no);
		cmd_inq.Parameters.Set("v_certi_bill_no", v_certi_bill_no);
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
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
