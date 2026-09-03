//框架头文件
#include "stdafx.h"
//程序用头文件


// service入口
BM2F_ENTERACE(qmts11_upd)
int f_qmts11_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	
	CString s_formname = "";
	CString s_steel_grade = "";
	CString s_judgment = "";
	int i_count = 0;
	
	CString v_update = "";
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tqmts("TQMTS11");
	

	CDbCommand cmd_inq(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts.Reset();
			tqmts.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			s_formname = s.formname;
			tqmts["SEQ_ID"] = bcls_rec->Tables[0].Rows[i]["SEQ_ID"];

			tqmts["REC_REVISE_TIME"] = datetime;
			tqmts["REC_REVISOR"] = s.userid;
			Log::Trace("", "", "{0}", bcls_rec->Tables[0].Rows[i]["SEQ_ID"].ToString());
			//v_update = "STEEL_GRADE,SPEC_MIN,SPEC_MAX,LESS_SPEC,MORE_SPEC,IN_SPEC,CASTING_PRE_JUDGMENT,MEASURE_DESC";
			tqmts.Update("*", "SEQ_ID");

		}
		Log::Trace("", "", "修改了{0}条记录", bcls_rec->Tables[0].Rows.get_Count());
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
