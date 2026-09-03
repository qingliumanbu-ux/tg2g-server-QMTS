/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 修改制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/*<remark>=========================================================
/// <summary>
/// 修改制造标准
/// <para>
/// 获取输入参数：st_no(出钢记号)
/// </para>
/// <para>数据库表：TQMTS02(工序成分标准表)
/// 前台各个QMTS0x画面的F3(修改)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各个制造标准表    </param>
===========================================================</remark>*/ 

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts_delchemi)


int f_qmts_delchemi(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;

	CString s_st_no = " ";
	CString s_whole_backlog_code = " ";
	CDecimal i_count = 0;
	CString base_code = "";

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");
	CModel tqmts0x("TQMTS0X");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		s_st_no					= bcls_rec->Tables[1].Rows[0]["st_no"].ToString().Trim();
		s_whole_backlog_code	= bcls_rec->Tables[1].Rows[0]["whole_backlog_code"].ToString().Trim();
		base_code               = bcls_rec->Tables[1].Rows[0]["base_code"].ToString().Trim();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[1].Rows[0]["factory_div"].ToString().TrimOrBlank();  ////update by yiling 20170224

		Log::Trace("", "", "qmts_delchemi IN: s_st_no[{0}]",s_st_no);
		Log::Trace("", "", "qmts_delchemi IN: s_whole_backlog_code[{0}]tqmts0x.FACTORY_DIV [{1}]base_code[{2}]", s_whole_backlog_code, tqmts0x["FACTORY_DIV"].ToString(), (const char*)base_code);

		if (s_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0006260")/*内部钢种不能为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tqmts0x["ST_NO"] = s_st_no;
		tqmts0x["BASE_CODE"] = base_code;
		tqmts0x.Query("ST_NO,FACTORY_DIV,BASE_CODE");
		////调用检查出钢记号是否在计划中使用的函数
		//bcls_rec_s.Tables[0].Rows.Add();
		//bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = "A";
		//bcls_rec_s.Tables[1].Rows.Add();
		//bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = s_st_no;
		//doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
		//if(doFlag != 0)
		//{    
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		//制造标准成分标准
		for (i = 0; i <bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tqmts02.Reset();
			tqmts02.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (tqmts02["ELM_CODE"].ToString().TrimOrBlank() == "")
			{
				sprintf(s.msg, _RES("QM00S0004168")/*元素代码不能为空。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts02["ST_NO"] = s_st_no;
			tqmts02["BASE_CODE"] = base_code;
			tqmts02["WHOLE_BACKLOG_CODE"] = s_whole_backlog_code;
			tqmts02.Delete("FACTORY_DIV,ST_NO,WHOLE_BACKLOG_CODE,ELM_CODE,BASE_CODE");
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "","error=[{0}]", (const char*)str );

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
