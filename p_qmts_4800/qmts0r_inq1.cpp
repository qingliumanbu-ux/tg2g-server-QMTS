/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   bhy
Version:    1.0
Date:     2024-08-26 9:13:56
Description: 质保书成分查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(qmts0r_inq1)

int f_qmts0r_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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


			sqlstr = " SELECT T.ELM_01_TC AS ELM_TC,T.ELM_01_MIN_TC AS ELM_MIN_TC,T.ELM_01_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_01 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";
			
			sqlstr += " UNION ALL SELECT T.ELM_02_TC AS ELM_TC,T.ELM_02_MIN_TC AS ELM_MIN_TC,T.ELM_02_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_02 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_03_TC AS ELM_TC,T.ELM_03_MIN_TC AS ELM_MIN_TC,T.ELM_03_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_03 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_04_TC AS ELM_TC,T.ELM_04_MIN_TC AS ELM_MIN_TC,T.ELM_04_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_04 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_05_TC AS ELM_TC,T.ELM_05_MIN_TC AS ELM_MIN_TC,T.ELM_05_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_05 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_06_TC AS ELM_TC,T.ELM_06_MIN_TC AS ELM_MIN_TC,T.ELM_06_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_06 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_07_TC AS ELM_TC,T.ELM_07_MIN_TC AS ELM_MIN_TC,T.ELM_07_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_07 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_08_TC AS ELM_TC,T.ELM_08_MIN_TC AS ELM_MIN_TC,T.ELM_08_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_08 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_09_TC AS ELM_TC,T.ELM_09_MIN_TC AS ELM_MIN_TC,T.ELM_09_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_09 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_10_TC AS ELM_TC,T.ELM_10_MIN_TC AS ELM_MIN_TC,T.ELM_10_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_10 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_11_TC AS ELM_TC,T.ELM_11_MIN_TC AS ELM_MIN_TC,T.ELM_11_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_11 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_12_TC AS ELM_TC,T.ELM_12_MIN_TC AS ELM_MIN_TC,T.ELM_12_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_12 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_13_TC AS ELM_TC,T.ELM_13_MIN_TC AS ELM_MIN_TC,T.ELM_13_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_13 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_14_TC AS ELM_TC,T.ELM_14_MIN_TC AS ELM_MIN_TC,T.ELM_14_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_14 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_15_TC AS ELM_TC,T.ELM_15_MIN_TC AS ELM_MIN_TC,T.ELM_15_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_15 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_16_TC AS ELM_TC,T.ELM_16_MIN_TC AS ELM_MIN_TC,T.ELM_16_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_16 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_17_TC AS ELM_TC,T.ELM_17_MIN_TC AS ELM_MIN_TC,T.ELM_17_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_17 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_18_TC AS ELM_TC,T.ELM_18_MIN_TC AS ELM_MIN_TC,T.ELM_18_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_18 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_19_TC AS ELM_TC,T.ELM_19_MIN_TC AS ELM_MIN_TC,T.ELM_19_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_19 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_20_TC AS ELM_TC,T.ELM_20_MIN_TC AS ELM_MIN_TC,T.ELM_20_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_20 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_21_TC AS ELM_TC,T.ELM_21_MIN_TC AS ELM_MIN_TC,T.ELM_21_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_21 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_22_TC AS ELM_TC,T.ELM_22_MIN_TC AS ELM_MIN_TC,T.ELM_22_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_22 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_23_TC AS ELM_TC,T.ELM_23_MIN_TC AS ELM_MIN_TC,T.ELM_23_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_23 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_24_TC AS ELM_TC,T.ELM_24_MIN_TC AS ELM_MIN_TC,T.ELM_24_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_24 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_25_TC AS ELM_TC,T.ELM_25_MIN_TC AS ELM_MIN_TC,T.ELM_25_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_25 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_26_TC AS ELM_TC,T.ELM_26_MIN_TC AS ELM_MIN_TC,T.ELM_26_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_26 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_27_TC AS ELM_TC,T.ELM_27_MIN_TC AS ELM_MIN_TC,T.ELM_27_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_27 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_28_TC AS ELM_TC,T.ELM_28_MIN_TC AS ELM_MIN_TC,T.ELM_28_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_28 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_29_TC AS ELM_TC,T.ELM_29_MIN_TC AS ELM_MIN_TC,T.ELM_29_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_29 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

			sqlstr += " UNION ALL SELECT T.ELM_30_TC AS ELM_TC,T.ELM_30_MIN_TC AS ELM_MIN_TC,T.ELM_30_MAX_TC AS ELM_MAX_TC,T.ELM_VALUE_30 AS ELM_VALUE "
				" FROM(SELECT * FROM QMTS0R WHERE 1 = 1  ";

			if (v_certi_print_no != "")
			{
				sqlstr += " AND  CERTI_PRINT_NO	= '" + v_certi_print_no + "'";
			}
			if (v_certi_bill_no != "")
			{
				sqlstr += " AND  CERTI_BILL_NO	= '" + v_certi_bill_no + "'";
			}
			sqlstr += " )T ";

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
