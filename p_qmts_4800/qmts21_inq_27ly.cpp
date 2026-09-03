/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      gx
Version:     1.0
Date:        2015-06-29
Description: 炉次下钢坯硫印实绩查询TQMTS27  低倍硫印标记=1
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



// service入口
BM2F_ENTERACE(qmts21_inq_27ly)


int f_qmts21_inq_27ly(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString s_heat_no = "";

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts27("TQMTS27");
	
	CDbCommand cmd_inq(conn);

	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_CONTENT");
	bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "ITEM_VALUES");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_JUDGE_FLAG");


	try
	{
		/*获得传入参数*/
		s_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();

		Log::Trace("", "", "qmts21_inq_27ly IN:---s_heat_no = [{0}]", s_heat_no);
		 
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
						"   FROM TQMTS27 "
						"  WHERE HEAT_NO  = @heat_no "
						"    AND ISE_TEST_FLAG = '1' ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no.Trim());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts27);
			CDataRow& row_ly1 = bcls_ret->Tables[0].Rows.Add();   //内质S_BAND A
			row_ly1["ITEM_CONTENT"] = "偏析 A";
			row_ly1["ITEM_VALUES"] = tqmts27["CENTER_SGRG_A"];
			row_ly1["ITEM_JUDGE_FLAG"] = tqmts27["SEGR_FLAG"];

			CDataRow& row_ly2 = bcls_ret->Tables[0].Rows.Add();   //内质S_BAND B
			row_ly2["ITEM_CONTENT"] = "偏析 B";
			row_ly2["ITEM_VALUES"] = tqmts27["CENTER_SGRG_B"];
			row_ly2["ITEM_JUDGE_FLAG"] = tqmts27["SEGR_FLAG"];

			CDataRow& row_ly3 = bcls_ret->Tables[0].Rows.Add();   //内质S_BAND C
			row_ly3["ITEM_CONTENT"] = "偏析 C";
			row_ly3["ITEM_VALUES"] = tqmts27["CENTER_SGRG_C"];
			row_ly3["ITEM_JUDGE_FLAG"] = tqmts27["SEGR_FLAG"];

			CDataRow& row_ly4 = bcls_ret->Tables[0].Rows.Add();   //内裂
			row_ly4["ITEM_CONTENT"] = "内裂";
			row_ly4["ITEM_VALUES"] = tqmts27["INTER_CRACK_GRADE"];
			row_ly4["ITEM_JUDGE_FLAG"] = tqmts27["INSIDE_FLAG"];

			CDataRow& row_ly5 = bcls_ret->Tables[0].Rows.Add();   //中心裂纹
			row_ly5["ITEM_CONTENT"] = "中裂";
			row_ly5["ITEM_VALUES"] = tqmts27["CRACK_CENTER"];
			row_ly5["ITEM_JUDGE_FLAG"] = tqmts27["CRACK_CENTER_FLAG"];

			CDataRow& row_ly6 = bcls_ret->Tables[0].Rows.Add();   //AL2O3夹杂
			row_ly6["ITEM_CONTENT"] = "夹杂1";
			row_ly6["ITEM_VALUES"] = tqmts27["CLUSTER_1"];
			row_ly6["ITEM_JUDGE_FLAG"] = tqmts27["AL2O3_FLAG"];

			CDataRow& row_ly7 = bcls_ret->Tables[0].Rows.Add();   //硅酸盐夹杂
			row_ly7["ITEM_CONTENT"] = "夹杂2";
			row_ly7["ITEM_VALUES"] = tqmts27["CLUSTER_2"];
			row_ly7["ITEM_JUDGE_FLAG"] = tqmts27["AL2O3_FLAG"];

			CDataRow& row_ly8 = bcls_ret->Tables[0].Rows.Add();   //总判定
			row_ly8["ITEM_CONTENT"] = "总判定";
			row_ly8["ITEM_VALUES"] = -1;
			row_ly8["ITEM_JUDGE_FLAG"] = tqmts27["JUDGE_CODE"];

			Log::Trace("", "", "judge_code = [{0}]", (const char*)tqmts27["JUDGE_CODE"].ToString());
		}
		cmd_inq.Close();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

