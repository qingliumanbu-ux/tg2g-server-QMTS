
//框架头文件
#include "stdafx.h"
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_tran_json_27(EIClass* blks_in, EIClass * blks_out, CDbConnection* conn);//调用

// service入口
BM2F_ENTERACE(qmts27_nq_inq)
BM2_FUNCTION_EXPORT
int f_qmts27_nq_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstrnq = "";
	//查询前一天的时间
	CString date = "";
	CString cs_mat_no = "";
	CString tap_date = " ";
	CDbCommand cmd_inq(conn);
	CString prod_shift = " ";
	CString prod_shift_no = " ";

	EIClass iplat_Tab;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_RECEIVE_WEIGHT");
	bcls_rec_s.Tables.Add();
	int   blkNum;


	try
	{
		
		Log::Trace("", "", "-----------------执行程序-------------");
		iplat_Tab.Tables.Clear();
		iplat_Tab.Tables.Add();
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "JSON");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "URL");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "COL");
		iplat_Tab.Tables[0].Rows.Add();


		cs_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		Log::Trace("", "", "LXX1[{0}]", cs_mat_no);
		Log::Trace("", "", "-----------------执行最新程序的识别-221-------------");
		iplat_Tab.Tables[0].Rows[0]["JSON"] = "{\"MatNo\":\"" + cs_mat_no + "\"}";
		iplat_Tab.Tables[0].Rows[0]["URL"] = "http://10.162.72.21:18816/getSouthSlab/queryByMatNo?MatNo=" + cs_mat_no;
		iplat_Tab.Tables[0].Rows[0]["COL"] = "data";
		Log::Trace("", "", "-----------------调用sql代理-------------");

		doFlag = f_tran_json_27(&iplat_Tab, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "LXX1[{0}]", doFlag);
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
