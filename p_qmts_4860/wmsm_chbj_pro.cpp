/************************/
/*** 2024-2-22 ********/
/****   wsl **************/
/**** 铸坯库存  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天


// service入口
BM2F_ENTERACE(wmsm_chbj_pro)
BM2_FUNCTION_EXPORT
int f_wmsm_chbj_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString date_r = CDateTime::Now().ToString("yyyyMMdd");
	CString str = "";
	CString str1 = "";
	CString date = "";
	CDecimal mat_wt = 0;
	CDecimal mat_wt_wt = 0;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_WT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_WT_WT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DATE_TIME");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "COUNT_HUIZO");

	try
	{
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
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			mat_wt = cmd_inq.GetDecimal(1);
			date = cmd_inq.GetString(2);
		}
		cmd_inq.Close();

		//铸坯库存 不锈钢
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr1 = "SELECT ROUND(SUM(MAT_ACT_WT))  FROM MMSM_MR_KC WHERE C_DIV = '1' ";
		}
		cmd_inq1.SetCommandText(sqlstr1);
		cmd_inq1.ExecuteReader();

		if (cmd_inq1.Read())
		{
			mat_wt_wt = cmd_inq1.GetDecimal(1);
		}
		cmd_inq1.Close();


		Log::Trace("", __FUNCTION__, "mat_wt[{0}]  ", mat_wt);
		Log::Trace("", __FUNCTION__, "mat_wt_wt[{0}]  ", mat_wt_wt);


		//发送宝武聊天
		if (mat_wt != 0 && mat_wt_wt != 0)
		{
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["MAT_WT"] = mat_wt;
			bcls_rec_s.Tables[0].Rows[0]["MAT_WT_WT"] = mat_wt_wt;
			bcls_rec_s.Tables[0].Rows[0]["DATE_TIME"] = date;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "3";
			bcls_rec_s.Tables[0].Rows[0]["REMARK"] = date_r;
			bcls_rec_s.Tables[0].Rows[0]["COUNT_HUIZO"] = mat_wt + mat_wt_wt;
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
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
