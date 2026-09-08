/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   bhy
Version:    1.0
Date:     2024-08-26 9:13:56
Description: 质保书导入查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(qmts0rdr_inq)

int f_qmts0rdr_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_order_no = "";
	CString v_heat_no = "";
	CString v_decide_code = "";
	CString decide_code_1 = "";
	CString v_table_name_1 = "";
	CString v_xyflag = "";
	CString v_userid = s.userid;
	CString v_userflag = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO"))
			v_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		/*if (bcls_rec->Tables[0].Columns.Contains("DECIDE_CODE"))
			v_decide_code = bcls_rec->Tables[0].Rows[0]["DECIDE_CODE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("DECIDE_CODE_1"))
			decide_code_1 = bcls_rec->Tables[0].Rows[0]["DECIDE_CODE_1"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME_1"))
			v_table_name_1 = bcls_rec->Tables[0].Rows[0]["TABLE_NAME_1"].ToString();*/
		if (bcls_rec->Tables[0].Columns.Contains("XY_FLAG"))
			v_xyflag = bcls_rec->Tables[0].Rows[0]["XY_FLAG"].ToString();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "v_heat_no =[{0}]", v_heat_no);
		Log::Info("", __FUNCTION__, "v_decide_code =[{0}]", v_decide_code);
		Log::Info("", __FUNCTION__, "v_table_name_1 =[{0}]", v_table_name_1);
		Log::Info("", __FUNCTION__, "v_userid =[{0}]", v_userid);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			cmd_inq_1.SetCommandText("select CODE_DESC_4_CONTENT from TWMSMZD02 WHERE CODE_CLASS = 'YH0RXY' AND CODE_DESC_1_CONTENT = '" + v_userid + "' ");
			cmd_inq_1.ExecuteReader();
			if (cmd_inq_1.Read()){
				Log::Info("", __FUNCTION__, "CODE_DESC_4_CONTENT =[{0}]", cmd_inq_1.GetString(1));
				if (cmd_inq_1.GetString(1) != ""){
					v_userflag = cmd_inq_1.GetString(1);
				}
			}
			cmd_inq_1.Close();
			Log::Info("", __FUNCTION__, "v_userflag =[{0}]", v_userflag);
			/*sqlstr = " SELECT * FROM ( SELECT T.*,DECODE(T.ELM_01_TC,'','0','1') AS XY_FLAG, "
				" CASE "
				" WHEN T.ELM_16_TC != ' ' OR T.ELM_VALUE_16 != ' ' THEN '2' "
				" ELSE '1' "
				" END  AS HS "
				" FROM( "
				" SELECT T1.*, T2.AYL_FLAG, T2.CHECK_FLAG, T2.DECIDE_CODE, T2.ELM_01_TC, T2.ELM_16_TC, T2.ELM_VALUE_16 FROM TQMTS0RDR T1 "
				" LEFT JOIN TQMTS0R05 T2 ON T1.HEAT_NO = T2.HEAT_NO AND T1.ORDER_NO = T2.ORDER_NO AND T1.NOW_ROW = T2.NOW_ROW)T) WHERE 1 = 1 ";*/

			
// DM8 适配 CHANGE-232:查询。空串搜索 DECODE(x,'',a,b) 改为标准 CASE WHEN x IS NULL OR x='' THEN a ELSE b,与 CHANGE-107 同理,不依赖空串/NULL 匹配的未记载语义。
// 改写原因：空串搜索 DECODE(x,'',a,b) 改为标准 CASE WHEN x IS NULL OR x='' THEN a ELSE b,与 CHANGE-107 同理,不依赖空串/NULL 匹配的未记载语义；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
			// sqlstr = "	SELECT * FROM ( "
				// " SELECT T.*, DECODE(T.ELM_01_TC, '', '0', '1') AS XY_FLAG,"
				// " CASE  WHEN T.ELM_16_TC != ' ' OR T.ELM_VALUE_16 != ' ' THEN '2'  ELSE '1'  END  AS HS"
				// " FROM("
				// " SELECT T1.*, T2.AYL_FLAG, T2.CHECK_FLAG, T2.DECIDE_CODE,"
				// " T2.ELM_VALUE_16,"
				// " T2.ELM_01_TC, T2.ELM_01_MIN_TC, T2.ELM_01_MAX_TC,"
				// " T2.ELM_02_TC, T2.ELM_02_MIN_TC, T2.ELM_02_MAX_TC,"
				// " T2.ELM_03_TC, T2.ELM_03_MIN_TC, T2.ELM_03_MAX_TC,"
				// " T2.ELM_04_TC, T2.ELM_04_MIN_TC, T2.ELM_04_MAX_TC,"
				// " T2.ELM_05_TC, T2.ELM_05_MIN_TC, T2.ELM_05_MAX_TC,"
				// " T2.ELM_06_TC, T2.ELM_06_MIN_TC, T2.ELM_06_MAX_TC,"
				// " T2.ELM_07_TC, T2.ELM_07_MIN_TC, T2.ELM_07_MAX_TC,"
				// " T2.ELM_08_TC, T2.ELM_08_MIN_TC, T2.ELM_08_MAX_TC,"
				// " T2.ELM_09_TC, T2.ELM_09_MIN_TC, T2.ELM_09_MAX_TC,"
				// " T2.ELM_10_TC, T2.ELM_10_MIN_TC, T2.ELM_10_MAX_TC,"
				// " T2.ELM_11_TC, T2.ELM_11_MIN_TC, T2.ELM_11_MAX_TC,"
				// " T2.ELM_12_TC, T2.ELM_12_MIN_TC, T2.ELM_12_MAX_TC,"
				// " T2.ELM_13_TC, T2.ELM_13_MIN_TC, T2.ELM_13_MAX_TC,"
				// " T2.ELM_14_TC, T2.ELM_14_MIN_TC, T2.ELM_14_MAX_TC,"
				// " T2.ELM_15_TC, T2.ELM_15_MIN_TC, T2.ELM_15_MAX_TC,"
				// " T2.ELM_16_TC, T2.ELM_16_MIN_TC, T2.ELM_16_MAX_TC,"
				// " T2.ELM_17_TC, T2.ELM_17_MIN_TC, T2.ELM_17_MAX_TC,"
				// " T2.ELM_18_TC, T2.ELM_18_MIN_TC, T2.ELM_18_MAX_TC,"
				// " T2.ELM_19_TC, T2.ELM_19_MIN_TC, T2.ELM_19_MAX_TC,"
				// " T2.ELM_20_TC, T2.ELM_20_MIN_TC, T2.ELM_20_MAX_TC,"
				// " T2.ELM_21_TC, T2.ELM_21_MIN_TC, T2.ELM_21_MAX_TC,"
				// " T2.ELM_22_TC, T2.ELM_22_MIN_TC, T2.ELM_22_MAX_TC,"
				// " T2.ELM_23_TC, T2.ELM_23_MIN_TC, T2.ELM_23_MAX_TC,"
				// " T2.ELM_24_TC, T2.ELM_24_MIN_TC, T2.ELM_24_MAX_TC,"
				// " T2.ELM_25_TC, T2.ELM_25_MIN_TC, T2.ELM_25_MAX_TC,"
				// " T2.ELM_26_TC, T2.ELM_26_MIN_TC, T2.ELM_26_MAX_TC,"
				// " T2.ELM_27_TC, T2.ELM_27_MIN_TC, T2.ELM_27_MAX_TC,"
				// " T2.ELM_28_TC, T2.ELM_28_MIN_TC, T2.ELM_28_MAX_TC," 
				// " T2.ELM_29_TC, T2.ELM_29_MIN_TC, T2.ELM_29_MAX_TC," 
				// " T2.ELM_30_TC, T2.ELM_30_MIN_TC, T2.ELM_30_MAX_TC" 
				// " FROM TQMTS0RDR T1"
				// " LEFT JOIN TQMTS0R05 T2 ON T1.HEAT_NO = T2.HEAT_NO AND T1.ORDER_NO = T2.ORDER_NO AND T1.NOW_ROW = T2.NOW_ROW)T"
				// " ) WHERE 1 = 1 ";
// DM8 SQL：
			sqlstr = "	SELECT * FROM ( "
				" SELECT T.*, CASE WHEN T.ELM_01_TC IS NULL OR T.ELM_01_TC = '' THEN '0' ELSE '1' END AS XY_FLAG,"
				" CASE  WHEN T.ELM_16_TC != ' ' OR T.ELM_VALUE_16 != ' ' THEN '2'  ELSE '1'  END  AS HS"
				" FROM("
				" SELECT T1.*, T2.AYL_FLAG, T2.CHECK_FLAG, T2.DECIDE_CODE,"
				" T2.ELM_VALUE_16,"
				" T2.ELM_01_TC, T2.ELM_01_MIN_TC, T2.ELM_01_MAX_TC,"
				" T2.ELM_02_TC, T2.ELM_02_MIN_TC, T2.ELM_02_MAX_TC,"
				" T2.ELM_03_TC, T2.ELM_03_MIN_TC, T2.ELM_03_MAX_TC,"
				" T2.ELM_04_TC, T2.ELM_04_MIN_TC, T2.ELM_04_MAX_TC,"
				" T2.ELM_05_TC, T2.ELM_05_MIN_TC, T2.ELM_05_MAX_TC,"
				" T2.ELM_06_TC, T2.ELM_06_MIN_TC, T2.ELM_06_MAX_TC,"
				" T2.ELM_07_TC, T2.ELM_07_MIN_TC, T2.ELM_07_MAX_TC,"
				" T2.ELM_08_TC, T2.ELM_08_MIN_TC, T2.ELM_08_MAX_TC,"
				" T2.ELM_09_TC, T2.ELM_09_MIN_TC, T2.ELM_09_MAX_TC,"
				" T2.ELM_10_TC, T2.ELM_10_MIN_TC, T2.ELM_10_MAX_TC,"
				" T2.ELM_11_TC, T2.ELM_11_MIN_TC, T2.ELM_11_MAX_TC,"
				" T2.ELM_12_TC, T2.ELM_12_MIN_TC, T2.ELM_12_MAX_TC,"
				" T2.ELM_13_TC, T2.ELM_13_MIN_TC, T2.ELM_13_MAX_TC,"
				" T2.ELM_14_TC, T2.ELM_14_MIN_TC, T2.ELM_14_MAX_TC,"
				" T2.ELM_15_TC, T2.ELM_15_MIN_TC, T2.ELM_15_MAX_TC,"
				" T2.ELM_16_TC, T2.ELM_16_MIN_TC, T2.ELM_16_MAX_TC,"
				" T2.ELM_17_TC, T2.ELM_17_MIN_TC, T2.ELM_17_MAX_TC,"
				" T2.ELM_18_TC, T2.ELM_18_MIN_TC, T2.ELM_18_MAX_TC,"
				" T2.ELM_19_TC, T2.ELM_19_MIN_TC, T2.ELM_19_MAX_TC,"
				" T2.ELM_20_TC, T2.ELM_20_MIN_TC, T2.ELM_20_MAX_TC,"
				" T2.ELM_21_TC, T2.ELM_21_MIN_TC, T2.ELM_21_MAX_TC,"
				" T2.ELM_22_TC, T2.ELM_22_MIN_TC, T2.ELM_22_MAX_TC,"
				" T2.ELM_23_TC, T2.ELM_23_MIN_TC, T2.ELM_23_MAX_TC,"
				" T2.ELM_24_TC, T2.ELM_24_MIN_TC, T2.ELM_24_MAX_TC,"
				" T2.ELM_25_TC, T2.ELM_25_MIN_TC, T2.ELM_25_MAX_TC,"
				" T2.ELM_26_TC, T2.ELM_26_MIN_TC, T2.ELM_26_MAX_TC,"
				" T2.ELM_27_TC, T2.ELM_27_MIN_TC, T2.ELM_27_MAX_TC,"
				" T2.ELM_28_TC, T2.ELM_28_MIN_TC, T2.ELM_28_MAX_TC," 
				" T2.ELM_29_TC, T2.ELM_29_MIN_TC, T2.ELM_29_MAX_TC," 
				" T2.ELM_30_TC, T2.ELM_30_MIN_TC, T2.ELM_30_MAX_TC" 
				" FROM TQMTS0RDR T1"
				" LEFT JOIN TQMTS0R05 T2 ON T1.HEAT_NO = T2.HEAT_NO AND T1.ORDER_NO = T2.ORDER_NO AND T1.NOW_ROW = T2.NOW_ROW)T"
				" ) WHERE 1 = 1 ";


			//2025.09.19 北区和特钢分权限查询导入信息
			if (v_userflag == '1')
			{
				sqlstr += " AND SUBSTR(HEAT_NO, 0, 1) != 'C'";
			}
			if (v_userflag == '2')
			{
				sqlstr += " AND SUBSTR(HEAT_NO, 0, 1) = 'C'";
			}

			if (v_order_no != "")
			{
				sqlstr += " AND  ORDER_NO	= '" + v_order_no + "'";
			}
			if (v_heat_no != "")
			{
				sqlstr += " AND  HEAT_NO	like '" + v_heat_no + "%'";
			}
			if (v_xyflag != "")
			{
				sqlstr += " AND  XY_FLAG	= '" + v_xyflag + "'";
			}

			sqlstr += " ORDER BY REC_CREATE_TIME DESC ";
			

			break;
		}
		cmd_inq.Parameters.Set("v_decide_code", v_decide_code);
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
		cmd_inq.Parameters.Set("v_order_no", v_order_no);
		cmd_inq.Parameters.Set("v_table_name_1", v_table_name_1);
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
