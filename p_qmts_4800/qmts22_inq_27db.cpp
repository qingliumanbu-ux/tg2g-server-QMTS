/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      gx
Version:     1.0
Date:        2015-06-29
Description: 炉次质量信息查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



// service入口
BM2F_ENTERACE(qmts22_inq_27db)


int f_qmts22_inq_27db(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString s_slab_no = "";

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
		s_slab_no = bcls_rec->Tables[0].Rows[0]["slab_no"].ToString().Trim();

		Log::Trace("", "", "qmts22_inq_27ly IN:---s_slab_no = [{0}]", s_slab_no);

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
						"   FROM TQMTS27 "
						"  WHERE SLAB_NO  = @slab_no "
						"    AND ISE_TEST_FLAG = '2' ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("slab_no", s_slab_no.Trim());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts27);
			Log::Trace("", "", "tqmts27.CENTER_SGRG_A = [{0}]", tqmts27["CENTER_SGRG_A"].ToDecimal().ToDouble());
			Log::Trace("", "", "tqmts27.SEGR_FLAG = [{0}]", (const char*)tqmts27["SEGR_FLAG"].ToString());

			CDataRow& row_db1 = bcls_ret->Tables[0].Rows.Add();   //偏析
			row_db1["ITEM_CONTENT"] = "偏析";
			row_db1["ITEM_VALUES"] = tqmts27["MACRO_CENTER_SEGR"];
			row_db1["ITEM_JUDGE_FLAG"] = tqmts27["SEGR_FLAG"];

			CDataRow& row_db2 = bcls_ret->Tables[0].Rows.Add();   //内裂
			row_db2["ITEM_CONTENT"] = "内裂";
			row_db2["ITEM_VALUES"] = tqmts27["MACRO_CRACK_INTERNAL"]; //inter_crack
			row_db2["ITEM_JUDGE_FLAG"] = tqmts27["INSIDE_FLAG"];

			CDataRow& row_db3 = bcls_ret->Tables[0].Rows.Add();   //夹杂
			row_db3["ITEM_CONTENT"] = "夹杂";
			row_db3["ITEM_VALUES"] = tqmts27["INCLU"];
			row_db3["ITEM_JUDGE_FLAG"] = tqmts27["AL2O3_FLAG"];

			CDataRow& row_db4 = bcls_ret->Tables[0].Rows.Add();   //三角区
			row_db4["ITEM_CONTENT"] = "三角区";
			row_db4["ITEM_VALUES"] = tqmts27["TRI_CRACK_GRADE"];
			row_db4["ITEM_JUDGE_FLAG"] = tqmts27["TRI_CRACK_FLAG"];

			CDataRow& row_db5 = bcls_ret->Tables[0].Rows.Add();   //角裂
			row_db5["ITEM_CONTENT"] = "角裂";
			row_db5["ITEM_VALUES"] = tqmts27["ANGLE_CRACK_GRADE"];
			row_db5["ITEM_JUDGE_FLAG"] = tqmts27["HORN_FLAG"];

			CDataRow& row_db6 = bcls_ret->Tables[0].Rows.Add();   //黑点
			row_db6["ITEM_CONTENT"] = "黑点";
			row_db6["ITEM_VALUES"] = tqmts27["MACULA"];
			row_db6["ITEM_JUDGE_FLAG"] = tqmts27["MACULA_FLAG"];

			CDataRow& row_db7 = bcls_ret->Tables[0].Rows.Add();   //等轴晶率
			row_db7["ITEM_CONTENT"] = "等轴晶率";
			row_db7["ITEM_VALUES"] = tqmts27["WAFER"];
			row_db7["ITEM_JUDGE_FLAG"] = tqmts27["WAFER_FLAG"];

			CDataRow& row_db8 = bcls_ret->Tables[0].Rows.Add();   //缩孔
			row_db8["ITEM_CONTENT"] = "疏孔";
			row_db8["ITEM_VALUES"] = tqmts27["HORE"];
			row_db8["ITEM_JUDGE_FLAG"] = tqmts27["HORE_FLAG"];

			CDataRow& row_db9 = bcls_ret->Tables[0].Rows.Add();   //负偏析
			row_db9["ITEM_CONTENT"] = "负偏析";
			row_db9["ITEM_VALUES"] = tqmts27["NEGSAND_MINUS"];
			row_db9["ITEM_JUDGE_FLAG"] = tqmts27["NEGSAND_MINUS_FLAG"];


			CDataRow& row_db10 = bcls_ret->Tables[0].Rows.Add();   //总判定
			row_db10["ITEM_CONTENT"] = "总判定";
			row_db10["ITEM_VALUES"] = -1;
			row_db10["ITEM_JUDGE_FLAG"] = tqmts27["JUDGE_CODE"];

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

