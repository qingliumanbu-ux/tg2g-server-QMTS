
/************************************************************/
/* 生成日期         2023-06-02                              */
/* 程序名           qmts21m_st_no_check                  */
/* Language                                           */
/* 功能		         出钢记号合否校验                    */
/* 调用方式         后台调用                                */
/* 生成人				                          */
//***********************************************************/  


//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中

BM2_FUNCTION_IMPORT
int f_qmts_jud(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);//成分判定

// service入口
BM2F_ENTERACE(qmts21m_st_no_check)


int f_qmts21m_st_no_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;

	CString s_heat_no = " ";
	CString s_fin_st_no = " ";
	CString s_st_sample_no = " ";
	CString s_pono = " ";
	CString sqlstr = " ";
	
	EIClass bcls_rec_f;
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	bcls_ret->Tables[0].Columns.Add(DT_STRING,"JUDGE_CODE");
	bcls_ret->Tables[0].Rows.Add();

	try
	{
		//获得输入参数
		s_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		s_fin_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		s_st_sample_no = bcls_rec->Tables[0].Rows[0]["st_sample_no"].ToString().Trim();
		s_pono = bcls_rec->Tables[0].Rows[0]["pono"].ToString().Trim();
		/* 检查输入参数合法性 */
		if (s_heat_no.Trim() == "")
		{
			sprintf(s.msg, "熔炼号不能为空.");
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数s_heat_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (s_st_sample_no.Trim() == "")
		{
			sprintf(s.msg, "试样号不能为空.");
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数s_st_sample_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS25";
		bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = s_heat_no;
		bcls_rec_f.Tables[0].Rows[0]["ST_SAMPLE_NO"] = s_st_sample_no;
		bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = s_fin_st_no;
		bcls_rec_f.Tables[0].Rows[0]["PONO"] = s_pono;
		doFlag = f_qmts_jud(&bcls_rec_f, &bcls_ret_f, conn);
		if (doFlag != 0)
		{
			Log::Trace("", __FUNCTION__, "f_qmts_jud() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		CString judge_code = bcls_ret_f.Tables[0].Rows[0]["JUDGE_CODE"].ToString().TrimOrBlank();
		bcls_ret->Tables[0].Rows[0]["JUDGE_CODE"] = judge_code;
		
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
