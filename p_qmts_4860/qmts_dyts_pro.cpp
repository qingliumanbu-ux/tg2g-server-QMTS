/************************/
/*** 2024-9-5 ********/
/****   xmy **************/
/**** 每班推送  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天

// service入口
BM2F_ENTERACE(qmts_dyts_pro)
BM2_FUNCTION_EXPORT
int f_qmts_dyts_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstrnq = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString date_r = CDateTime::Now().ToString("yyyyMMdd");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inqtest(conn);
	CString prod_shift = " ";
	CString prod_shift_no = " ";
	CString tap_date = " ";
	CString datetime1 = CDateTime::Now().AddDays(-1).ToString("yyyyMMddHHmmss");
	CString content = " ";
	CString topic = " ";
	EIClass iplat_Tab;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "TAP_DATE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CONTENT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "TOPIC");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");


	try
	{
		tap_date = datetime.SubstringNE(0, 8);
		Log::Trace("", "", "tap_date{0}", tap_date);
		//每班推送 北区
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select TOPIC,CONTENT from TQMTSBWDY where DATE_CODE='"+tap_date+"' ";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			topic = cmd_inq.GetString(1);
			content = cmd_inq.GetString(2);
		}
		if (content != " "){
			//发送宝武聊天
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "16";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			//发送宝武聊天
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "17";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			//发送宝武聊天
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "18";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "19";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "20";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "21";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "22";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "23";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			//最后一次寶物推送
			bcls_rec_s.Tables[0].Rows.Clear();
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
			bcls_rec_s.Tables[0].Rows[0]["TOPIC"] = topic;
			bcls_rec_s.Tables[0].Rows[0]["CONTENT"] = content;
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "24";
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
		}
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
