/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(qmts11_add)

int f_qmts11_add(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString s_formname = "";
	CString s_steel_grade = "";
	CString s_judgment = "";
	int i_count=0;
	
	CDecimal p_seq_id = 0;
	
	CModel tqmts("TQMTS11");
	
	CDbCommand cmd_inq(conn);
	try
	{
		
		s_formname = s.formname;
		
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts.Reset();
			tqmts.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			/*****2024-1-11 wcm 变更主键为seq_id  start*******/
			CDbCommand getSeq1("SELECT max(seq_id)+1  FROM TQMTS11", conn);
			p_seq_id = getSeq1.ExecuteScalar();
			if (p_seq_id.ToString().GetLength() > 18)p_seq_id = 0;
			tqmts["SEQ_ID"] = p_seq_id;
			
			
			/*****2018-1-24 wzn 变更主键为seq_id  end*******/
			tqmts["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts["REC_CREATOR"] = s.userid;
			tqmts.Print();
			tqmts.TrimOrBlank();

			
					
		   tqmts.Insert();
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