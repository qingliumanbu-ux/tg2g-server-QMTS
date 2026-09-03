/************************/
/*** 2024-2-20 ********/
/****   wsl **************/
/**** 修磨量  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天


// service入口
BM2F_ENTERACE(mmsm_xium_pro)
BM2_FUNCTION_EXPORT
int f_mmsm_xium_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString date_r = CDateTime::Now().ToString("yyyyMMdd");
	CString after_wt = " ";
	CString count_ks = " ";
	CString date_to = "";
	CDbCommand cmd_inq(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MEND_AFTER_WEIGHT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "COUNT_KS");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DATE_TIME_TO");



	try
	{
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
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			after_wt = cmd_inq.GetString(1);
			count_ks = cmd_inq.GetString(2);
			date_to = cmd_inq.GetString(3);
		}
		cmd_inq.Close();
		//发送宝武聊天
		if (after_wt != "0" && count_ks != "0")
		{
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["MEND_AFTER_WEIGHT"] = after_wt;
			bcls_rec_s.Tables[0].Rows[0]["COUNT_KS"] = count_ks;
			bcls_rec_s.Tables[0].Rows[0]["REMARK"] = date_r;
			bcls_rec_s.Tables[0].Rows[0]["DATE_TIME_TO"] = date_to;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "5";
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
