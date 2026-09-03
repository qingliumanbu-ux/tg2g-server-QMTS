/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 删除制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中












/*<remark>=========================================================
/// <summary>
/// 删除制造标准
/// <para>
/// 获取输入参数：table_name(制造标准表名)/st_no(出钢记号)/whole_backlog_code(炼钢工序)/flag(删除标记——0:制造标准;1:成分标准)
/// </para>
/// <para>数据库表：TQMTS0x(各个制造标准表) 
/// 				TQMTS02(工序成分标准表)
/// 				TQMTS01(工序制造标准表)
/// 前台各个QMTS0x画面的F5(删除)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各个制造标准表    </param>
===========================================================</remark>*/ 

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts_del)


int f_qmts_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int flag = 0;

	CString table_name = " ";
	CString q_st_no = " ";
	CString q_whole_backlog_code = " ";
	CDecimal v_count = 0;
	CString s_equ_no = " ";
	CString s_ingot_code = " ";
	CString base_code = "";

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts02("TQMTS02");
	CModel tqmts03("TQMTS03");
	CModel tqmts04("TQMTS04");
	CModel tqmts05("TQMTS05");
	CModel tqmts05p("TQMTS05P");
	CModel tqmts06("TQMTS06");
	CModel tqmts07("TQMTS07");
	CModel tqmts08("TQMTS08");
	CModel tqmts0a("TQMTS0A");
	CModel tqmts0m("TQMTS0M");
	CModel tqmts0l("TQMTS0L");
	CModel tqmts0x("TQMTS0X");
	CModel tep0002("TEP0002");


	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_del(conn);

	try
	{
		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().ToLower().Trim();
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		//flag = bcls_rec->Tables[0].Rows[0]["flag"].ToDecimal().ToInt16();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();  ////update by yiling 20170224
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
		Log::Trace("", "", "qmts_del IN: table_name[{0}]st_no[{1}]whole_backlog_code[{2}]flag[{3}]tqmts0x.FACTORY_DIV[{4}]base_code[{5}]", (const char*)table_name, (const char*)q_st_no, (const char*)q_whole_backlog_code, flag, tqmts0x["FACTORY_DIV"].ToString(), (const char*)base_code);

		if(q_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tqmts0x["ST_NO"] = q_st_no;
		tqmts0x["BASE_CODE"] = base_code;
		tqmts0x.Query("ST_NO,FACTORY_DIV,BASE_CODE");
		tep0002["CODE_CLASS"] = "QMZ7";
		tep0002["CODE_DESC_4_CONTENT"] = table_name.ToUpper();
		tep0002.Query();
		//q_whole_backlog_code = tep0002["CODE"];
		Log::Trace("", "", "q_whole_backlog_code[{0}]", q_whole_backlog_code);

		//调用检查出钢记号是否在计划中使用的函数——add by 冯晓轶 2012-03-23
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		bcls_rec_s.Tables[1].Rows.Add();
		bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = q_st_no;
		doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
		if(doFlag != 0)
		{    
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if(flag == 0)
		{
			//制造标准
			if(table_name == "tqmts03")
			{
				tqmts03["ST_NO"] = q_st_no;
				tqmts03["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts03["BASE_CODE"] = base_code;
				tqmts03.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
			}
			if(table_name == "tqmts04")
			{
				tqmts04["ST_NO"] = q_st_no;
				tqmts04["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts04["BASE_CODE"] = base_code;
				tqmts04.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
			}
			if(table_name == "tqmts05")
			{
				tqmts05["ST_NO"] = q_st_no;
				tqmts05["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts05["BASE_CODE"] = base_code;
				tqmts05.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
			}
			if (table_name == "tqmts05p")
			{
				tqmts05p["ST_NO"] = q_st_no;
				tqmts05p["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts05p["BASE_CODE"] = base_code;
				tqmts05p.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
			}
			if(table_name == "tqmts06")
			{
				tqmts06["ST_NO"] = q_st_no;
				tqmts06["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts06["BASE_CODE"] = base_code;
				tqmts06.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
			}
			if(table_name == "tqmts07")
			{
				tqmts07["ST_NO"] = q_st_no;
				tqmts07["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts07["BASE_CODE"] = base_code;
				tqmts07.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
			}
			if(table_name == "tqmts08")
			{
				tqmts08["ST_NO"] = q_st_no;
				tqmts08["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts08["BASE_CODE"] = base_code;
				tqmts08["EQU_NO"] = s_equ_no;
				tqmts08["INGOT_CODE"] = s_ingot_code;
				tqmts08.Delete("ST_NO,FACTORY_DIV,BASE_CODE,EQU_NO,INGOT_CODE");
			}
			if(table_name == "tqmts0a")
			{
				tqmts0a["ST_NO"] = q_st_no;
				tqmts0a["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts0a["BASE_CODE"] = base_code;
				tqmts0a.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
			}
			if(table_name == "tqmts0m")
			{
				tqmts0m["ST_NO"] = q_st_no;
				tqmts0m["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts0m["BASE_CODE"] = base_code;
				tqmts0m["EQU_NO"] = s_equ_no;
				tqmts0m["INGOT_CODE"] = s_ingot_code;
				tqmts0m.Delete("ST_NO,FACTORY_DIV,BASE_CODE,EQU_NO,INGOT_CODE");
			}

			if (table_name == "tqmts0l")
			{
				tqmts0l["ST_NO"] = q_st_no;
				tqmts0l["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts0l["BASE_CODE"] = base_code;
				tqmts0l.Delete("ST_NO,FACTORY_DIV,BASE_CODE");
			}
			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:						// 所有数据库适用，通用SQL语句
			//		sqlstr = "DELETE " + table_name +
			//				 " WHERE ST_NO = @st_no ";
			//		break;
			//}
			//Log::Trace("", "","st_no = [{0}],sqlstr = [{1}]",(const char*)sqlstr,(const char*)q_st_no);
			//cmd_del.SetCommandText(sqlstr);
			//cmd_del.Parameters.Set("st_no",q_st_no);
			//cmd_del.ExecuteScalar( );
			//cmd_del.Close();
		}
		//else if(flag == 1)
		//{
		//	//制造标准成分标准
		//	for (i = 0; i <bcls_rec->Tables[1].Rows.get_Count(); i++ )
		//	{
		//		//取得单行传入信息
		//		tqmts02.Reset();
		//		tqmts02.MergeFrom(bcls_rec->Tables[1].Rows[i]);

		//		if(tqmts02["ELM_CODE"].ToString().TrimOrBlank() == "")
		//		{
		//			sprintf(s.msg,_RES("QM00S0004168")/*元素代码不能为空。*/);
		//			throw CApplicationException(-1, s.msg, s.svc_name);
		//		}

		//		tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		//		tqmts02["ST_NO"] = q_st_no;
		//		tqmts02["WHOLE_BACKLOG_CODE"] = q_whole_backlog_code;
		//		tqmts02["BASE_CODE"] = base_code;
  //  			tqmts02.Delete("FACTORY_DIV,ST_NO,WHOLE_BACKLOG_CODE,ELM_CODE,BASE_CODE");
		//	}
		//}

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
						 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code"
			        	 "   AND BASE_CODE =@base_code"
						 "   AND FACTORY_DIV =@factory_div";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no",q_st_no);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("whole_backlog_code",q_whole_backlog_code);
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		v_count = cmd_inq.ExecuteScalar();

		if (v_count > 0 )
		{
			tqmts01["ST_NO"] = q_st_no;
			tqmts01["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts01["BASE_CODE"] = base_code;
			tqmts01["WHOLE_BACKLOG_CODE"] = q_whole_backlog_code;
			tqmts01.Delete("ST_NO,FACTORY_DIV,BASE_CODE,WHOLE_BACKLOG_CODE");

			Log::Trace("", "", "st_no[{0}]whole_backlog_code[{1}]tqmts01.FACTORY_DIV[{2}]", tqmts01["ST_NO"].ToString(), tqmts01["WHOLE_BACKLOG_CODE"].ToString(), tqmts0x["FACTORY_DIV"].ToString());

			//tqmts01.Update("IF_PASS,IF_MESSAGE,MESSAGE_TIME,FIN_CONFM_MAKER,FIN_CONFM_TIME,RES_CODE,REC_REVISOR,REC_REVISE_TIME",  //修改字段项
			//				"ST_NO,WHOLE_BACKLOG_CODE,FACTORY_DIV,BASE_CODE"); //条件字段项
			Log::Trace("", "", "upd end");
		}
		cmd_inq.Close();
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
