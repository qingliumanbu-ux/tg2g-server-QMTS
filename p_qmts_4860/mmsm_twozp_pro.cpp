/************************/
/*** 2024-3-19 ********/
/****   wsl **************/
/**** 2#LF中频炉虚拟料仓库存  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天


// service入口
BM2F_ENTERACE(mmsm_twozp_pro)
BM2_FUNCTION_EXPORT
int f_mmsm_twozp_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd_inq(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "TWO_KGPS");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");

	try
	{
		//2#LF中频炉虚拟料仓库存
		CString two_kgps = bcls_rec->Tables[0].Rows[0]["TWO_KGPS"].ToString().Trim();
		CString remark = bcls_rec->Tables[0].Rows[0]["REMARK"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "two_kgps[{0}]  ", two_kgps);

		//发送宝武聊天
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["TWO_KGPS"] = two_kgps;
		bcls_rec_s.Tables[0].Rows[0]["CODE"] = "";
		bcls_rec_s.Tables[0].Rows[0]["REMARK"] = remark;
		Log::Trace("", "", "REMARK={0}", remark);
		doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
		if (doFlag < 0)
		{
			strcpy(s.msg, "调用函数报错!");
			throw CApplicationException(-1, s.msg, log.Location);
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
