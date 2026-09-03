/************************/
/*** 2024-3-19 ********/
/****   sw **************/
/**** 双基管理  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat_dd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天


// service入口
BM2F_ENTERACE(mmsm_sjgl_pro)
BM2_FUNCTION_EXPORT
int f_mmsm_sjgl_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString daySeq = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd_inq(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK_LK");

	CModel tqmtsbwsj("TQMTSBWSJ");

	try
	{
	
		sqlstr = "SELECT DAYS, CONTENT FROM TQMTSBWSJ WHERE SEND_FLAG='0' ORDER BY DAYS ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			//发送宝武聊天
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "25";
			bcls_rec_s.Tables[0].Rows[0]["REMARK"] = datetime + "-" + cmd_inq.GetString(1);
			bcls_rec_s.Tables[0].Rows[0]["REMARK_LK"] = cmd_inq.GetString(2);
			Log::Trace("", __FUNCTION__, "REMARK_LK[{0}]  ", bcls_rec_s.Tables[0].Rows[0]["REMARK_LK"].ToString());
			daySeq = cmd_inq.GetString(1);
			doFlag = f_push_baowu_chat_dd(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmtsbwsj["DAYS"] = daySeq;
			if ("14" == daySeq)
			{
				sqlstr = " UPDATE TQMTSBWSJ set SEND_FLAG = '0' where 1=1 ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();
			}
			else
			{
				tqmtsbwsj["SEND_FLAG"] = "1";
				tqmtsbwsj.Update("SEND_FLAG", "DAYS");
			}
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