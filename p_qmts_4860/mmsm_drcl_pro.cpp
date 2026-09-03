/************************/
/*** 2024-2-20 ********/
/****   wsl **************/
/**** 当日产量  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天

int f_push_baowu_chat_dd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天（群聊）
// service入口
BM2F_ENTERACE(mmsm_drcl_pro)
BM2_FUNCTION_EXPORT
int f_mmsm_drcl_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString date_r = CDateTime::Now().ToString("yyyyMMdd");
	CDecimal c_receive_weight = 0;
	CDecimal s_receive_weight = 0;
	CDecimal c_receive_weight_lj = 0;
	CDecimal s_receive_weight_lj = 0;
	CString date = "";
	CString mat_no = "";
	CString after_wt = " ";
	CString count_ks = " ";
	CString date_to = "";
	CDecimal mat_wt = 0;
	CDecimal mat_wt_wt = 0;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CDbCommand cmd_inq4(conn);
	CDbCommand cmd_inq5(conn);
	CDbCommand cmd_inq6(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_RECEIVE_WEIGHT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "S_RECEIVE_WEIGHT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "RECV_TIME");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "ZO_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_RECEIVE_WEIGHT_LJ");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "S_RECEIVE_WEIGHT_LJ");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "ZO_COUNT_LJ");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "COUNT_KS");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DATE_TIME_TO");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_WT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_WT_WT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "COUNT_HUIZO");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MEND_AFTER_WEIGHT");
	
	

	try
	{
		//碳钢当日产量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  ROUND(SUM(MAT_ACT_WT)) MAT_ACT_WT, substr(to_date(sysdate - 1), 0, 2) RECV_TIME FROM VMMSMTS_CGCL";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			c_receive_weight = cmd_inq.GetDecimal(1);
			date = cmd_inq.GetString(2);
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "c_receive_weight[{0}]  ", c_receive_weight);
		Log::Trace("", __FUNCTION__, "date[{0}]  ", date);

		//碳钢累计产量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  ROUND(SUM(MAT_ACT_WT)/10000,2) MAT_ACT_WT  FROM VMMSMTS_CGCL_LJ";
		}
		cmd_inq2.SetCommandText(sqlstr);
		cmd_inq2.ExecuteReader();
		if (cmd_inq2.Read())
		{
			c_receive_weight_lj = cmd_inq2.GetDecimal(1);
		}
		cmd_inq2.Close();


		//不锈钢当日产量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  ROUND(SUM(MAT_ACT_WT)) MAT_ACT_WT FROM VMMSMTS_BXGCL";
		}
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.ExecuteReader();
		if (cmd_inq1.Read())
		{
			s_receive_weight = cmd_inq1.GetDecimal(1);
		}
		cmd_inq1.Close();

		Log::Trace("", __FUNCTION__, "s_receive_weight[{0}]  ", s_receive_weight);

		//不锈钢累计产量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  ROUND(SUM(MAT_ACT_WT)/10000,2) MAT_ACT_WT  FROM VMMSMTS_BXGCL_LJ";
		}
		cmd_inq3.SetCommandText(sqlstr);
		cmd_inq3.ExecuteReader();
		if (cmd_inq3.Read())
		{
			s_receive_weight_lj = cmd_inq3.GetDecimal(1);
		}
		cmd_inq3.Close();

		//修磨量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT ROUND(SUM(MEND_BEFORE_WEIGHT)), COUNT(MEND_BEFORE_WEIGHT), substr(to_date(sysdate - 1), 0, 2) DATE_TIME_TO "
				"FROM TMMSM34 WHERE SUBSTR(REC_CREATE_TIME, 1, 8) = TO_CHAR(to_date(sysdate - 1), 'YYYYMMDD')";
		}
		cmd_inq4.SetCommandText(sqlstr);
		cmd_inq4.ExecuteReader();
		if (cmd_inq4.Read())
		{
			after_wt = cmd_inq4.GetString(1);
			count_ks = cmd_inq4.GetString(2);
			date_to = cmd_inq4.GetString(3);
		}
		cmd_inq4.Close();

		//铸坯库存 碳钢
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT ROUND(SUM(MAT_ACT_WT)), substr(to_date(sysdate - 1), 0, 2) DATE_TIME_TO  FROM MMSM_MR_KC WHERE C_DIV = '2'";
		}
		cmd_inq5.SetCommandText(sqlstr);
		cmd_inq5.ExecuteReader();
		if (cmd_inq5.Read())
		{
			mat_wt = cmd_inq5.GetDecimal(1);
			date = cmd_inq5.GetString(2);
		}
		cmd_inq5.Close();

		//铸坯库存 不锈钢
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT ROUND(SUM(MAT_ACT_WT))  FROM MMSM_MR_KC WHERE C_DIV = '1' ";
		}
		cmd_inq6.SetCommandText(sqlstr);
		cmd_inq6.ExecuteReader();

		if (cmd_inq6.Read())
		{
			mat_wt_wt = cmd_inq6.GetDecimal(1);
		}
		cmd_inq6.Close();


		if (c_receive_weight != 0 || s_receive_weight != 0)
		{
			//发送宝武聊天
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["C_RECEIVE_WEIGHT"] = c_receive_weight;//碳钢当日产量
			bcls_rec_s.Tables[0].Rows[0]["S_RECEIVE_WEIGHT"] = s_receive_weight;//不锈钢当日产量
			bcls_rec_s.Tables[0].Rows[0]["RECV_TIME"] = date;
			bcls_rec_s.Tables[0].Rows[0]["ZO_COUNT"] = c_receive_weight + s_receive_weight;//总产量
			bcls_rec_s.Tables[0].Rows[0]["C_RECEIVE_WEIGHT_LJ"] = c_receive_weight_lj;//碳钢累计产量
			bcls_rec_s.Tables[0].Rows[0]["S_RECEIVE_WEIGHT_LJ"] = s_receive_weight_lj;//不锈钢累计产量
			bcls_rec_s.Tables[0].Rows[0]["ZO_COUNT_LJ"] = c_receive_weight_lj + s_receive_weight_lj;//累计总产量
			bcls_rec_s.Tables[0].Rows[0]["DATE_TIME_TO"] = date_to;
			bcls_rec_s.Tables[0].Rows[0]["MEND_AFTER_WEIGHT"] = after_wt;
			bcls_rec_s.Tables[0].Rows[0]["COUNT_KS"] = count_ks;
			bcls_rec_s.Tables[0].Rows[0]["MAT_WT"] = mat_wt;
			bcls_rec_s.Tables[0].Rows[0]["MAT_WT_WT"] = mat_wt_wt;
			bcls_rec_s.Tables[0].Rows[0]["COUNT_HUIZO"] = mat_wt + mat_wt_wt;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "2";
			bcls_rec_s.Tables[0].Rows[0]["REMARK"] = date_r;
			/*doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			doFlag = f_push_baowu_chat_dd(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
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
