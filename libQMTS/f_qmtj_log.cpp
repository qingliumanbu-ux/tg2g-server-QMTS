/**********************************************************************/
/* 功能说明        : 写材料处置履历			                         */
/* 程序对应表名    : tqmtjlg				                          */
/**********************************************************************/

//框架公用头文件，勿删
#include "stdafx.h"


BM2_FUNCTION_EXPORT
 int f_qmtj_log(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	//程序用头文件
	int  doFlag = 0;
	int  fetchRowCount;
	int  blkSeq;
	CString QMTJBlock;

	CModel tqmtjlg("TQMTJLG");
	CDbCommand cmd_inq(conn);
	CString sqlstr;

	CTracer log(__FUNCTION__);
	try
	{
		tqmtjlg.Reset();

		//在service里已经add过了
		QMTJBlock = "QMTJLOG";
		blkSeq = bcls_rec->AtBlkName(QMTJBlock);
		if (blkSeq<=0) 
		{
			blkSeq = bcls_rec->AddBlock();
			bcls_rec->SetBlkName(blkSeq,QMTJBlock); 
		} 
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)  ///update by yiling 20170509  支持多行压入
		{
		//取得单行传入信息
		tqmtjlg.MergeFrom(bcls_rec->Tables[QMTJBlock].Rows[i]);

		Log::Trace("", "", "f_qmtj_log---tqmtjlg.MAT_NO[{0}],tqmtjlg.COMPLEX_DECIDE_CODE[{1}]",(const char*)tqmtjlg["MAT_NO"].ToString(),(const char*)tqmtjlg["COMPLEX_DECIDE_CODE"].ToString());
		Log::Trace("", "", "f_qmtj_log---tqmtjlg.DEAL_CODE[{0}],",(const char*)tqmtjlg["DEAL_CODE"].ToString());

		//取系统18位时间做序列号
		tqmtjlg["COMPLEX_SEQ_NO"] =CDateTime::Now().ToString("yyyyMMddHHmmssfff");

		Log::Trace("", "","序列号[{0}]",(const char*)tqmtjlg["COMPLEX_SEQ_NO"].ToString());



		tqmtjlg["REC_CREATOR"] = s.userid;
		tqmtjlg["REC_CREATE_TIME"]=CDateTime::Now().ToString("yyyyMMddHHmmss");
		tqmtjlg["REC_REVISOR"] = " ";
		tqmtjlg["REC_REVISE_TIME"] = " ";
		tqmtjlg["ARCHIVE_FLAG"] = " ";


		tqmtjlg.Insert();
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
