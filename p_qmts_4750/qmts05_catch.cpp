/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 获取工艺卡制造标准－VD/VOD
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/*<remark>=========================================================
/// <summary>
/// 获取工艺卡制造标准－VD/VOD
/// <para>
/// 获取输入参数：TQMTS04(制造标准_VD/VOD表) ；
/// </para>
/// <para>数据库表：TQMTS04(制造标准_VD/VOD表)); 
/// 前台QMTS04画面的F9(获取工艺卡)调用    </para>
/// </summary>
/// <param name="TQMTS04">制造标准_VD/VOD表    </param>
===========================================================</remark>*/  

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用
int f_qmtsp_01(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//获取成分标准

// service入口
BM2F_ENTERACE(qmts05_catch)

int f_qmts05_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	CString s_st_no = " ";
	CString base_code = "";
	CString s_whole_backlog_code = " ";
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
	CModel tqmts05("TQMTS05");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//取得单行传入信息
		tqmts05.Reset();
		s_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		s_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
	//	s_whole_backlog_code = "V";
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
		doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
		if(doFlag != 0)
		{    
			throw CApplicationException(-1, s.msg, log.Location);
		}

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
						 "  FROM TQMTS05 "
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
		if (cmd_inq.Read())	//记录已存在
		{
			//获取修改前的数据
			cmd_inq.Fetch(tqmts05);
			s_rec_creator = tqmts05["REC_CREATOR"];
			s_rec_create_time = tqmts05["REC_CREATE_TIME"];
			i_version = tqmts05["VERSION"].ToDecimal() + 1;
		}
		else//记录不存在
		{
			/******************** 赋初值 *************************/
			s_rec_creator = s.userid;
			s_rec_create_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
			i_version = tqmts05["VERSION"];
		}
		cmd_inq.Close();

		/******************** 赋值 *************************/
		tqmts05.CopyFrom(tqmts0x);

		tqmts05["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tqmts05["ARCHIVE_FLAG"] = " ";
		tqmts05["DU_FLAG"] = " ";
		tqmts05["DU_MAKER"] = " ";
		tqmts05["DU_TIME"] = " ";
		tqmts05["REC_REVISOR"] = s.userid;
		tqmts05["CATCH_RESP"] = s.userid;
		tqmts05["CATCH_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//将工艺卡信息更新到制造标准
		tqmts05["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		tqmts05["BASE_CODE"] = base_code;
		tqmts05.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
		tqmts05.TrimOrBlank();
		tqmts05.Insert();

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
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("whole_backlog_code", s_whole_backlog_code);
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
