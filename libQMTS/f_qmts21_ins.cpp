/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   HYF
Version:    1.0
Date:     2013-05-27 09:13:56
Description: 炼钢炉次异常写入TQMTS21品质异常管理表
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中






/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炼钢炉次等级计算
/// <para>
/// 1.炼钢转炉区域炉次异常判断
/// 
/// </para>
/// <para>数据库表：TQMTS21
///												                </para>
/// <para>主调用函数： 1.f_qmts_yc01 f_qmts_yc02 f_qmts_yc03  炼钢各工序炉次异常处理函数

/// <para>需调用函数：
///          
/// </summary>
/// <param name="heat_no"> 熔炼号             </param>
/// <param name="WHOLE BACKLOG_CODE"> 工序代码             </param>
/// <param name="ABNY_CODE"> 异常代码           </param>
/// <param name="ST_NO"> 钢种           </param>
/// 1.获取/检查传入参数, 合理性检查:熔炼号、工序代码不允许为空.
/// 2.根据传入参数，查找相关表，获得异常处置信息
/// 3.写入TQMTS21表
/// 4.计算炉次等级

/// <returns>  </returns>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */


BM2_FUNCTION_EXPORT
 int f_qmts21_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	//APP_BEGIN()
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	CString sqlstr("");
	int doFlag = 0;
	int ret = 0;
	int v_count = 0;
	CString v_st_no="";
	CString v_heat_grade="";
	CDecimal a=0;
	CString v_proc_no="";
	EIClass inBlock;
	EIClass outBlock;

	/* 实体类定义 */
	CModel tqmts21("TQMTS21");
	CModel tqmts11("TQMTS11");
	CModel tqmts12("TQMTS12");
	CModel tqmts18("TQMTS18");


	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_del(conn);

	try
	{	

		//定义函数传入参数
		//inBlock.Tables[0].Columns.Add(DT_STRING,"PONO");   //制造命令号
		//inBlock.Tables[0].Columns.Add(DT_STRING,"PROC_NO");   //处理号
		//inBlock.Tables[0].Columns.Add(DT_STRING,"HEAT_GRADE"); //炉次等级
		//inBlock.Tables[0].Columns.Add(DT_STRING,"WHOLE_BACKLOG_CODE");//工序



		//-----------------------------------------------------------
		/* 获得输入参数 */
		tqmts21["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();//熔炼号
		tqmts21["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();//熔炼号
		tqmts21["WHOLE_BACKLOG_CODE"] = bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"].ToString().Trim();//工序代码
		tqmts21["ABNY_CODE"] = bcls_rec->Tables[0].Rows[0]["ABNY_CODE"].ToString().Trim();//异常代码
		v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();//钢种
		tqmts21["PROC_NO"] =bcls_rec->Tables[0].Rows[0]["PROC_NO"].ToString().Trim();

		if(tqmts21["PROC_NO"].ToString() =="")
		{
			tqmts21["PROC_NO"]=" ";
		}
		Log::Trace("", __FUNCTION__, "PONO=[{0}]", (const char*)tqmts21["PONO"].ToString());
		Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}]",(const char*)tqmts21["HEAT_NO"].ToString()		);
		Log::Trace("", __FUNCTION__, "WHOLE_BACKLOG_CODE= 	  [{0}]",(const char*)tqmts21["WHOLE_BACKLOG_CODE"].ToString()		);
		Log::Trace("", __FUNCTION__, "ST_NO= [{0}]",(const char*)v_st_no);
		Log::Trace("", __FUNCTION__, "PROC_NO= [{0}]",(const char*)tqmts21["PROC_NO"].ToString());
		Log::Trace("", __FUNCTION__, "ABNY_CODE= [{0}]",(const char*)tqmts21["ABNY_CODE"].ToString());


		if(tqmts21["ABNY_CODE"].ToString() !="")  
		{

			//根据异常代码，查询异常内容
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default: // 所有数据库适用，通用SQL语句
				sqlstr = " SELECT ABNR_DESC,ABNY_LV "
					" FROM TQMTS18 "
					" WHERE  ABNY_CODE = @abny_code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);			
			cmd_inq.Parameters.Set("abny_code", tqmts21["ABNY_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts21["ABN_CONT"] = cmd_inq.GetString(1);
				tqmts21["ABN_SERS_GRADE"] = cmd_inq.GetDecimal(2);
			}else
			{
				tqmts21["ABN_CONT"] ="异常项目未配置";
				tqmts21["ABN_SERS_GRADE"] =99;
			}
			cmd_inq.Close();


			//-----------------------------------------------------------
			//根据出钢记号，查询异常处理组号
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default: // 所有数据库适用，通用SQL语句
				sqlstr = " SELECT ABNR_TREAT_GRP_CODE "
					" FROM TQMTS08 "
					" WHERE  ST_NO = @st_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);			
			//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
			cmd_inq.Parameters.Set("st_no", v_st_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts11["ABNR_TREAT_GRP_CODE"] = cmd_inq.GetDecimal(1);
			
				//根据异常处理组号、异常代码、炉次铸坯区分查询异常处置代码
				switch(conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default: // 所有数据库适用，通用SQL语句
					sqlstr = " SELECT ABN_DEAL "
						" FROM TQMTS11 "
						" WHERE  CHARGE_CB_DIV = @charge_cb_div "
						" AND  ABNR_TREAT_GRP_CODE =@abnr_treat_grp_code "
						" AND  ABNY_CODE =@abny_code ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);			
				cmd_inq.Parameters.Set("charge_cb_div", "0");
				cmd_inq.Parameters.Set("abnr_treat_grp_code", tqmts11["ABNR_TREAT_GRP_CODE"].ToDecimal());
				cmd_inq.Parameters.Set("abny_code", tqmts21["ABNY_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts21["ABN_DEAL"] = cmd_inq.GetString(1);
				}else
				{
					tqmts21["ABN_DEAL"] ="0";
				}
				cmd_inq.Close();



				//根据处置代码，查询炉次处置内容

				switch(conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default: // 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						" FROM TQMTS12 "
						" WHERE  AB_TREAT = @ab_treat ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);			
				cmd_inq.Parameters.Set("ab_treat", tqmts21["ABN_DEAL"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tqmts12);
				}
				cmd_inq.Close();


				Log::Trace("", "", "tqmts12.OK_FLAG[{0}], tqmts12.RETAIM[{1}], tqmts12.ST_CHANG[{2}]", tqmts12["OK_FLAG"].ToString(), tqmts12["RETAIM"].ToString(), tqmts12["ST_CHANG"].ToString());

				//写炉次处置内容 1 通过，2变更 ，3保留
				if(tqmts12["OK_FLAG"].ToString() =="1")
				{
					tqmts21["ABN_DEAL_REMARK"] ="通过";
					a = a.Parse(tqmts21["ABNY_CODE"].ToString());
					Log::Trace("", __FUNCTION__, "a=[{0}]",a.ToInt32());
					v_heat_grade ="10"+ tqmts21["ABNY_CODE"].ToString().Format("%.2d",a.ToInt32());
					Log::Trace("", __FUNCTION__, "v_heat_grade=[{0}]",(const char*)v_heat_grade);

				}else  if(tqmts12["RETAIM"].ToString() =="1")
				{
					tqmts21["ABN_DEAL_REMARK"] ="保留";
					a = a.Parse(tqmts21["ABNY_CODE"].ToString());
					Log::Trace("", __FUNCTION__, "a=[{0}]",a.ToInt32());
					v_heat_grade ="30"+ tqmts21["ABNY_CODE"].ToString().Format("%.2d",a.ToInt32());
					Log::Trace("", __FUNCTION__, "v_heat_grade=[{0}]",(const char*)v_heat_grade);

				}else if(tqmts12["ST_CHANG"].ToString() =="1")
				{
					tqmts21["ABN_DEAL_REMARK"] ="钢种变更";
					a = a.Parse(tqmts21["ABNY_CODE"].ToString());
					Log::Trace("", __FUNCTION__, "a=[{0}]",a.ToInt32());
					v_heat_grade ="20"+ tqmts21["ABNY_CODE"].ToString().Format("%.2d",a.ToInt32());
					Log::Trace("", __FUNCTION__, "v_heat_grade=[{0}]",(const char*)v_heat_grade);

				}else
				{
					tqmts21["ABN_DEAL_REMARK"] ="其他";

					v_heat_grade ="0000";
					Log::Trace("", __FUNCTION__, "v_heat_grade=[{0}]",(const char*)v_heat_grade);

				}

				tqmts21["HEAT_GRADE"]=v_heat_grade;

				Log::Trace("", __FUNCTION__, "此炉号此工序此缺陷如有过，则先删,key:tqmts21.PONO[{0}]tqmts21.WHOLE_BACKLOG_CODE[{1}]tqmts21.ABNY_CODE[{2}]", tqmts21["PONO"].ToString(), tqmts21["WHOLE_BACKLOG_CODE"].ToString(), tqmts21["ABNY_CODE"].ToString());
			
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " DELETE FROM TQMTS21"
						"  WHERE PONO = @tqmts21.PONO "
						"    AND WHOLE_BACKLOG_CODE = @tqmts21.WHOLE_BACKLOG_CODE "
						"    AND ABNY_CODE = @tqmts21.ABNY_CODE ";
				}
				cmd_del.SetCommandText(sqlstr);
				cmd_del.Parameters.Set("tqmts21.PONO", tqmts21["PONO"].ToString());
				cmd_del.Parameters.Set("tqmts21.WHOLE_BACKLOG_CODE", tqmts21["WHOLE_BACKLOG_CODE"].ToString());
				cmd_del.Parameters.Set("tqmts21.ABNY_CODE", tqmts21["ABNY_CODE"].ToString());
				cmd_del.ExecuteNonQuery();

				Log::Trace("", __FUNCTION__, "此炉号此工序此缺陷删除成功。");

				//插入炉次品质异常管理表
				tqmts21.Insert();
			}
			else  //TQMTS08连铸制造标准表中无记录
			{
				//tqmts11["ABNR_TREAT_GRP_CODE"]= 0;
				Log::Trace("", __FUNCTION__, "此内部钢种没有连铸工序,或者连铸工序没有配异常处理组号，不计算异常和炉次等级。");
				strcpy(s.msg, "此内部钢种没有连铸工序,或者连铸工序没有配异常处理组号，不计算异常和炉次等级。");
				//throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();

		}else //约定：传入异常代码为空时，就做删除
		{
			//每次调用时，删除同处理号的异常信息
			Log::Trace("", __FUNCTION__, "每次调用时，删除同处理号的异常信息");
			tqmts21.Delete("HEAT_NO,PROC_NO,WHOLE_BACKLOG_CODE");

		}


		//---END

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
	//在函数退出前，统一Close()操作
	cmd_inq.Close();

	return doFlag;
}

