/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 审核制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中





/*<remark>=========================================================
/// <summary>
/// 审核制造标准
/// <para>
/// 获取输入参数：TQMTS01(工序制造标准表) ；
/// </para>
/// <para>数据库表：TQMTS01(工序制造标准表)
/// 前台各个QMTS0x画面的F10(审核)调用    </para>
/// </summary>
/// <param name="TQMTS01">工序制造标准表    </param>
===========================================================</remark>*/ 


// service入口
BM2F_ENTERACE(qmts_pass)

int f_qmts_pass(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	CString  sqlstr("");   
	int doFlag = 0;
	int i;

	CString table_name = " ";
	CString s_st_no = " ";
	CString s_whole_backlog_code = " ";
	CString base_code = "";
	CDecimal i_count = 0;
	CDecimal i_count_cc = 0;
	CString s_equ_no = " ";
	CString s_ingot_code = " ";
	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts08("TQMTS08");
	CModel tqmts0x("TQMTS0X");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/*获得传入参数*/
		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim();
		s_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		s_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();  ////update by yiling 20170224

		Log::Trace("", "", "qmts_pass IN: table_name[{0}]st_no[{1}]whole_backlog_code[{2}]tqmts0x.FACTORY_DIV[{3}]base_code[{4}]", (const char*)table_name, (const char*)s_st_no, (const char*)s_whole_backlog_code, tqmts0x["FACTORY_DIV"].ToString(), (const char*)base_code);
		if (bcls_rec->Tables[0].Columns.Contains("equ_no"))
		{
			s_equ_no = bcls_rec->Tables[0].Rows[0]["equ_no"].ToString().TrimOrBlank();
			Log::Trace("", "", "s_equ_no[{0}]", s_equ_no);
		}
		if (bcls_rec->Tables[0].Columns.Contains("ingot_code"))
		{
			s_ingot_code = bcls_rec->Tables[0].Rows[0]["ingot_code"].ToString().TrimOrBlank();
			Log::Trace("", "", "s_ingot_code[{0}]", s_ingot_code);
		}
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1) "
						 "  FROM TQMTS01 "
						 " WHERE ST_NO = @st_no "
					     " AND BASE_CODE =@base_code"
						 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
						 " AND FACTORY_DIV=@factory_div";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no",s_st_no);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("whole_backlog_code",s_whole_backlog_code);
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		i_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (i_count <= 0)
		{
			CFormattable arguments[] = {(const char*)s_st_no,(const char*)s_whole_backlog_code};
			CMessageFormat::Format(s.msg, _RES("QM00S0003975")/*内部钢种[{0}]在工艺卡确认中无工序[{1}]。*/, arguments, 2);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1) "
						 "  FROM TQMTS02 "
						 " WHERE ST_NO = @st_no "
					      " AND BASE_CODE =@base_code"
						 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
						 " AND FACTORY_DIV=@factory_div";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no",s_st_no);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("whole_backlog_code",s_whole_backlog_code);
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		i_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (i_count <= 0)
		{
			strcpy(s.msg,_RES("QM00S0005492")/*审核失败：该出钢记号制造标准缺少成分标准。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if(table_name == "TQMTS08")
		{
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT *  "
							 "  FROM TQMTS08 "
							 " WHERE ST_NO = @st_no "
					         " AND BASE_CODE =@base_code"
							 " AND FACTORY_DIV=@factory_div"
							 " AND EQU_NO=@equ_no AND INGOT_CODE=@ingot_code"
							 ;
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no", s_st_no);
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("equ_no", s_equ_no.TrimOrBlank());
			cmd_inq.Parameters.Set("ingot_code", s_ingot_code.TrimOrBlank());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts08);
			}
			cmd_inq.Close();

			if(tqmts08["CAST_SPEED_STD"].ToDecimal() <= 0)
			{
				strcpy(s.msg,_RES("QM00S0004172")/*连铸标准铸造速度:数值必须大于0。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if(tqmts08["CUT_HEAD_LEN_H"].ToDecimal() <= 0)
			{
				strcpy(s.msg,_RES("QM00S0004173")/*头部切头长:数值必须大于0。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if(tqmts08["CUT_HEAD_LEN_B"].ToDecimal() <= 0)
			{
				strcpy(s.msg,_RES("QM00S0004174")/*尾部切头长:数值必须大于0。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		/******************** 赋初值 *************************/
		tqmts01["IF_PASS"] = "1";
		tqmts01["FIN_CONFM_MAKER"] = s.userid;
		tqmts01["FIN_CONFM_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tqmts01["ST_NO"] = s_st_no;
		tqmts01["BASE_CODE"] = base_code;
		tqmts01["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		tqmts01["WHOLE_BACKLOG_CODE"] = s_whole_backlog_code;

		tqmts01.Update("IF_PASS,FIN_CONFM_MAKER,FIN_CONFM_TIME",	//修改字段项
						"ST_NO,WHOLE_BACKLOG_CODE,FACTORY_DIV,BASE_CODE"); //条件字段项
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
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
