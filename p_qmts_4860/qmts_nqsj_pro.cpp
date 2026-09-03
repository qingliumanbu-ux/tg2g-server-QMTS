/************************/
/*** 2024-9-19 ********/
/****   xmy **************/
/**** 南区每班数据  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_tran_json_func(EIClass* blks_in, EIClass * blks_out, CDbConnection* conn);//调用

// service入口
BM2F_ENTERACE(qmts_nqsj_pro)
BM2_FUNCTION_EXPORT
int f_qmts_nqsj_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstrnq = "";
	//查询前一天的时间
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString datetime1 = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");
	CDecimal c_receive_weight = 0;
	CDecimal s_receive_weight = 0;
	CDecimal c_receive_weight_lj = 0;
	CDecimal s_receive_weight_lj = 0;
	CString date = "";
	CString mat_no = "";
	CString tap_date = " ";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CDbCommand cmd_inqz(conn);
	CModel tmmsmnqsj("TMMSMNQSJ");
	CString prod_shift = " ";
	CString prod_shift_no = " ";

	EIClass iplat_Tab;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_RECEIVE_WEIGHT");
	bcls_rec_s.Tables.Add();
	int   blkNum;


	try
	{
		tap_date = datetime.SubstringNE(0, 8);
		prod_shift = datetime.SubstringNE(8, 2);
		Log::Trace("", "", "prod_shift{0}", prod_shift);
		if (prod_shift == "00"){
			prod_shift_no = "二";
			tap_date = datetime1.SubstringNE(0, 8);
		}
		if (prod_shift == "16" || prod_shift == "17"){
			prod_shift_no = "白";
		}
		if (prod_shift == "08"){
			prod_shift_no = "夜";
		}
		Log::Trace("", "", "-----------------执行程序-------------");
		iplat_Tab.Tables.Clear();
		iplat_Tab.Tables.Add();
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "JSON");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "URL");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "COL");
		iplat_Tab.Tables[0].Rows.Add();


		Log::Trace("", "", "-----------------执行最新程序的识别-221-------------", tap_date);
		iplat_Tab.Tables[0].Rows[0]["JSON"] = "{\"tappingDate\":\""+tap_date+"\"}";
		iplat_Tab.Tables[0].Rows[0]["URL"] = "http://10.162.72.21:18816/getSouthProductInfo/findSckProductInfo?tappingDate=" + tap_date + "";
		iplat_Tab.Tables[0].Rows[0]["COL"] = "data";
		Log::Trace("", "", "-----------------调用sql代理-------------");

		doFlag = f_tran_json_func(&iplat_Tab, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "LXX1[{0}]", doFlag);
		}
		//删除

		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			//Log::Trace("", "", "--共[{0}]行，第[{1}]行数据---", bcls_ret->Tables[0].Rows.get_Count(), i);

			if (i > 15)
			{
				break;
			}
			if (bcls_ret->Tables[0].Rows[i]["grindStartShiftno"].ToString().Trim() == prod_shift_no){
				tmmsmnqsj["PROD_DATE"] = bcls_ret->Tables[0].Rows[i]["tapDate"].ToString().Trim();
				tmmsmnqsj["CLASSGROUP"] = bcls_ret->Tables[0].Rows[i]["grindStartShiftno"].ToString().Trim();
				tmmsmnqsj["BOF_CNUMBER"] = bcls_ret->Tables[0].Rows[i]["bofcnumber"].ToString().Trim();
				tmmsmnqsj["SI_NUMBER"] = bcls_ret->Tables[0].Rows[i]["sinumber"].ToString().Trim();
				tmmsmnqsj["FE_NUMBER"] = bcls_ret->Tables[0].Rows[i]["fenumber"].ToString().Trim();
				tmmsmnqsj["BOF_SNUMBER"] = bcls_ret->Tables[0].Rows[i]["BOFSNUMBER"].ToString().Trim();
				tmmsmnqsj["MOLD_NUMBER"] = bcls_ret->Tables[0].Rows[i]["MOLDNUMBER"].ToString().Trim();
				tmmsmnqsj["CCM_CNUMBER"] = bcls_ret->Tables[0].Rows[i]["CCMCNUMBER"].ToString().Trim();
				tmmsmnqsj["CCM_SNUMBER"] = bcls_ret->Tables[0].Rows[i]["CCMSNUMBER"].ToString().Trim();
				tmmsmnqsj["SUM_COU"] = bcls_ret->Tables[0].Rows[i]["sumcou"].ToString().Trim();
				tmmsmnqsj["SUM_WT"] = bcls_ret->Tables[0].Rows[i]["sumwt"].ToString().Trim();
				tmmsmnqsj["JK_COUNT"] = bcls_ret->Tables[0].Rows[i]["jkcount"].ToString().Trim();
				tmmsmnqsj["JK_SUM"] = bcls_ret->Tables[0].Rows[i]["jksum"].ToString().Trim();
				tmmsmnqsj["RH_NUMBER"] = bcls_ret->Tables[0].Rows[i]["rhnumber"].ToString().Trim();
				tmmsmnqsj["S_JK_CAR"] = bcls_ret->Tables[0].Rows[i]["sjkcar"].ToString().Trim();
				tmmsmnqsj["S_JK_COUNT"] = bcls_ret->Tables[0].Rows[i]["sjkcount"].ToString().Trim();
				tmmsmnqsj["S_JK_SUM"] = bcls_ret->Tables[0].Rows[i]["sjksum"].ToString().Trim();
				tmmsmnqsj["C_JK_CAR"] = bcls_ret->Tables[0].Rows[i]["cjkcar"].ToString().Trim();
				tmmsmnqsj["C_JK_COUNT"] = bcls_ret->Tables[0].Rows[i]["cjkcount"].ToString().Trim();
				tmmsmnqsj["C_JK_SUM"] = bcls_ret->Tables[0].Rows[i]["cjksum"].ToString().Trim();
				tmmsmnqsj.Insert();
			}
			/*for (int t = 0; t < bcls_ret->Tables[0].Columns.get_Count(); t++)
			{
				//Log::Trace("", "", "--列名：[{0}],值：[{1}]---", bcls_ret->Tables[0].Columns[t].get_ColumnName(), bcls_ret->Tables[0].Rows[i][t].ToString());
				
			}*/
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
