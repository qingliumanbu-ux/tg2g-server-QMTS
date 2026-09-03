/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 获取工艺卡制造标准－连铸
**************************************************/

//框架公用头文件
#include "stdafx.h"

//程序用头文件




/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用
int f_qmtsp_01(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//获取成分标准

/*<remark>=========================================================
/// <summary>
/// 获取工艺卡制造标准－连铸
/// <para>
/// 获取输入参数：TQMTS08(制造标准_连铸表) ；
/// </para>
/// <para>数据库表：TQMTS08(制造标准_连铸表)); 
/// 前台QMTS08画面的F9(获取工艺卡)调用    </para>
/// </summary>
/// <param name="TQMTS08">制造标准_连铸表    </param>
===========================================================</remark>*/  
// service入口
BM2F_ENTERACE(qmts08_catch)

int f_qmts08_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	CString s_st_no = " ";
	CString s_whole_backlog_code = " ";
	CString base_code = "";
	CDecimal v_count = 0;

	CString s_rec_creator = " ";
	CString s_rec_create_time = " ";
	CDecimal i_version = 0;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	EIClass bcls_rec_f;
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"st_no");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"factory_div");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"whole_backlog_code");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "base_code");
	
	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts0x("TQMTS0X");
	CModel tqmts08("TQMTS08");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//取得单行传入信息
		tqmts08.Reset();
		s_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		s_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		//s_whole_backlog_code = "C";
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();  ////update by yiling 20170224

		Log::Trace("", "", "qmts_ins IN: st_no[{0}]whole_backlog_code[{1}]tqmts0x.FACTORY_DIV[{2}]", (const char*)s_st_no, (const char*)s_whole_backlog_code, tqmts0x["FACTORY_DIV"].ToString());

		if(s_st_no.Trim() == "")
		{
			strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//调用检查出钢记号是否在计划中使用的函数
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		bcls_rec_s.Tables[1].Rows.Add();
		bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = s_st_no;
		//doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
		//if(doFlag != 0)
		//{    
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
						 "  FROM TQMTS0X "
						 " WHERE ST_NO = @st_no "
					" AND BASE_CODE =@base_code"
						 " AND FACTORY_DIV=@factory_div";

				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", s_st_no);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts0x);
		}
		else
		{
			CFormattable arguments[] = {(const char*)s_st_no};
			CMessageFormat::Format(s.msg, _RES("QM00S0004000")/*出钢记号[{0}]在工艺卡中不存在，不能操作。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
						 "  FROM TQMTS08 "
						 " WHERE ST_NO = @st_no "
					" AND BASE_CODE =@base_code"
						 " AND FACTORY_DIV=@factory_div";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", s_st_no);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{			
			//获取修改前的数据
			cmd_inq.Fetch(tqmts08);
			CString v_ingot_code = tqmts08["INGOT_CODE"];
			tqmts08.CopyFrom(tqmts0x);
			tqmts08["INGOT_CODE"] = v_ingot_code;  //INGOT_CODE在TQMTS0X中存在
			tqmts08["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts08["REC_REVISOR"] = s.userid;
			tqmts08["CATCH_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts08["CATCH_RESP"] = s.userid;
			tqmts08["ARCHIVE_FLAG"] = " ";
			tqmts08["DU_FLAG"] = " ";
			tqmts08["DU_MAKER"] = " ";
			tqmts08["DU_TIME"] = " ";
			
			//将工艺卡信息更新到制造标准
			tqmts08["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts08["BASE_CODE"] = base_code;
			tqmts08.Delete("ST_NO,FACTORY_DIV,EQU_NO,INGOT_CODE,BASE_CODE");
			tqmts08.TrimOrBlank();
			tqmts08.Insert();
		}
		cmd_inq.Close();

		//修改工艺卡确认－生效标记
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1) "
						 "  FROM TQMTS01 "
						 " WHERE ST_NO = @st_no"
					" AND BASE_CODE =@base_code"
						 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code"
						 " AND FACTORY_DIV=@factory_div";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no",s_st_no);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("whole_backlog_code",s_whole_backlog_code);
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		v_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (v_count > 0 )
		{
			tqmts01["IF_PASS"] = "0";
			tqmts01["IF_MESSAGE"] = " ";
			tqmts01["MESSAGE_TIME"] = " ";
			tqmts01["FIN_CONFM_MAKER"] = " ";
			tqmts01["FIN_CONFM_TIME"] = " ";
			tqmts01["RES_CODE"] = " ";
			tqmts01["REC_REVISOR"] = s.userid;
			tqmts01["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts01["ST_NO"] = s_st_no;
			tqmts01["BASE_CODE"] = base_code;
			tqmts01["WHOLE_BACKLOG_CODE"] = s_whole_backlog_code;
			tqmts01["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts01.Update("IF_PASS",  //修改字段项
							"ST_NO,WHOLE_BACKLOG_CODE,FACTORY_DIV,BASE_CODE"); //条件字段项
		}


		/******************************获取成分标准**********************************/
		bcls_rec_f.Tables[0].Rows.Add();
		bcls_rec_f.Tables[0].Rows[0]["st_no"] = s_st_no;
		bcls_rec_f.Tables[0].Rows[0]["factory_div"] = tqmts0x["FACTORY_DIV"];
		bcls_rec_f.Tables[0].Rows[0]["whole_backlog_code"] = s_whole_backlog_code;
		bcls_rec_f.Tables[0].Rows[0]["base_code"] = base_code;
		doFlag = f_qmtsp_01(&bcls_rec_f,&bcls_ret_f,conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
