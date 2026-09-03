/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 任龙
日期: 2012-03-16
功能: 氧化铁粉实绩信息新增
修改历史：
	日期:________；修改人：________; 需求提出人________
	变更内容:

**************************************************/
/*<remark>=========================================================
/// <summary>
/// 氧化铁粉实绩信息新增
/// <para>
/// 1. 读取前台传入参数；
/// 2. 获取表列名；
/// 3. 建立新增语句；
/// 4. 执行新增操作，返回新增结果。
/// </para>
/// </summary>
/// <param name="TABLE_NAME">界面名称</param>
/// <param name="sql_ins">拼接sql语句</param>
/// <returns>新增基表信息</returns>
===========================================================</remark>*/


#include "stdafx.h"


BM2F_ENTERACE(qmtifa_ins);


int f_qmtifa_ins(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	// 系统日志类定义
	CTracer log(__FUNCTION__);

    // 程序内部用变量
	int         doFlag                   =     0;
	int         logFlag                  =     1;

	size_t      row_count                =     0;
	size_t      col_count                =     0;
	size_t      colIdx                   =     0;
	
    // 获取当前时间
	CString     datetimeNow             = CDateTime::Now().ToString("yyyyMMddHHmmss");


	 // 时间计算
	struct timeval tpstart, tpend;
	double      timediff;

    // 记录开始时间
    gettimeofday(&tpstart, NULL);

    CString     strSqlCols              =       "";
	CString     strSqlValues            =       "";

	CString     lpsz_column_name        =       "";
    DsType      dsType ;
	CString     sql_value               =       "";
	CString     strSql                  =       "";

    // ----------------------------------------
    // 程序公共CDbCommand
    CDbCommand cmd(conn);

    // 程序开始
	try
	{
        // ----------------------------------------
	    /*获取传入行数*/
		row_count    = bcls_rec->Tables[0].Rows.get_Count();
		col_count    = bcls_rec->Tables[0].Columns.get_Count();

	    strSqlCols   = " REC_CREATOR,REC_CREATE_TIME,REC_REVISOR ,REC_REVISE_TIME,GRADE," 
			           " DLVY_STOCK_FLAG,DLVY_STOCK_TIME,DLVY_STOCK_ID,JUDGE_MAKER_ID,"
					   " JUDGE_TIME,ALTER_JUDGE_MAKER,ALTER_JUDGE_TIME,REINSPECT_FLAG,"
					   " SEND_FLAG,SEND_TIME,SEND_ID ";

        strSqlValues = " @rec_creator,@rec_create_time,@rec_revisor ,@rec_revise_time,@grade," 
			           " @dlvy_stock_flag,@dlvy_stock_time,@dlvy_stock_id,@judge_maker_id,"
					   " @judge_time,@alter_judge_maker,@alter_judge_time,@reinspect_flag,"
					   " @send_flag,@send_time,@send_id ";
			// ----------------------------------------
			// 拼接sql语句
			for(size_t colidex = 0;colidex < col_count; colidex++)
			{
				 /*拼接列名*/
				 lpsz_column_name    = bcls_rec->Tables[0].Columns[colidex].get_ColumnName();
				  // 类型
				 dsType              = bcls_rec->Tables[0].Columns[colIdx].get_DataType();

				 // 跳过公共字段
				 if (   ( lpsz_column_name == "REC_CREATOR"          )
					 || ( lpsz_column_name == "REC_CREATE_TIME"      )
					 || ( lpsz_column_name == "REC_REVISOR"          )
					 || ( lpsz_column_name == "REC_REVISE_TIME"      )
					 || ( lpsz_column_name == "GRADE"                )
					 || ( lpsz_column_name == "DLVY_STOCK_FLAG"      )
					 || ( lpsz_column_name == "DLVY_STOCK_TIME"      )
					 || ( lpsz_column_name == "DLVY_STOCK_ID"        )
					 || ( lpsz_column_name == "JUDGE_MAKER_ID"       )
					 || ( lpsz_column_name == "JUDGE_TIME"           )
					 || ( lpsz_column_name == "ALTER_JUDGE_MAKER"    )
					 || ( lpsz_column_name == "ALTER_JUDGE_TIME"     )
					 || ( lpsz_column_name == "REINSPECT_FLAG"       )
					 || ( lpsz_column_name == "SEND_FLAG"            )
					 || ( lpsz_column_name == "SEND_TIME"            )
					 || ( lpsz_column_name == "SEND_ID"              )

					)
				 {
					continue ;
				 }	

				 // INSERT语句
				strSqlCols += ", ";
				strSqlCols += lpsz_column_name.ToUpper();

				//VALUES 语句
				strSqlValues +=", @";
				strSqlValues += lpsz_column_name.ToLower();
			}

			strSqlCols   = strSqlCols.Substring(1);
			strSqlValues = strSqlValues.Substring(1);
			Log::Trace("","","新增列{0}",strSqlCols);
			Log::Trace("","","[%s]",(const char*)strSqlValues);
			// ----------------------------------------

			// 生成完整的sql语句
			strSql = " INSERT INTO TQMTIFA" "(" + strSqlCols + ") VALUES (" + strSqlValues + ") " ;

			// 修改公共参数
			cmd.Parameters.Set("rec_creator",       s.userid    );
			cmd.Parameters.Set("rec_create_time",   datetimeNow );
			cmd.Parameters.Set("rec_revisor",       s.userid    );
			cmd.Parameters.Set("rec_revise_time",   datetimeNow );
			cmd.Parameters.Set("grade",             " "         );
			cmd.Parameters.Set("dlvy_stock_flag",   " "         );
			cmd.Parameters.Set("dlvy_stock_time",   " "         );
			cmd.Parameters.Set("dlvy_stock_id",     " "         );
			cmd.Parameters.Set("judge_maker_id",    " "         );
			cmd.Parameters.Set("judge_time",        " "         );
			cmd.Parameters.Set("alter_judge_maker", " "         );
			cmd.Parameters.Set("alter_judge_time",  " "         );
			cmd.Parameters.Set("reinspect_flag",    " "         );
			cmd.Parameters.Set("send_flag",         " "         );
			cmd.Parameters.Set("send_time",         " "         );
			cmd.Parameters.Set("send_id",           " "         );
			// ----------------------------------------
			for (size_t rowIdx = 0; rowIdx < row_count; rowIdx++)
			{
				// 循环列
				for (size_t colIdx = 0; colIdx < col_count; colIdx++)
				{
					// 列名
					lpsz_column_name    = bcls_rec->Tables[0].Columns[colIdx].get_ColumnName().ToUpper();
					// 类型
					dsType              = bcls_rec->Tables[0].Columns[colIdx].get_DataType();
					// 数据值
					sql_value           = bcls_rec->Tables[0].Rows[rowIdx][colIdx].ToString().Trim();

					// 公共字段
					if (    ( lpsz_column_name == "REC_CREATOR"          )
						 || ( lpsz_column_name == "REC_CREATE_TIME"      )
						 || ( lpsz_column_name == "REC_REVISOR"          )
						 || ( lpsz_column_name == "REC_REVISE_TIME"      )
						 || ( lpsz_column_name == "GRADE"                )
						 || ( lpsz_column_name == "DLVY_STOCK_FLAG"      )
						 || ( lpsz_column_name == "DLVY_STOCK_TIME"      )
						 || ( lpsz_column_name == "DLVY_STOCK_ID"        )
						 || ( lpsz_column_name == "JUDGE_MAKER_ID"       )
						 || ( lpsz_column_name == "JUDGE_TIME"           )
						 || ( lpsz_column_name == "ALTER_JUDGE_MAKER"    )
						 || ( lpsz_column_name == "ALTER_JUDGE_TIME"     )
						 || ( lpsz_column_name == "REINSPECT_FLAG"       )
						 || ( lpsz_column_name == "SEND_FLAG"            )
						 || ( lpsz_column_name == "SEND_TIME"            )
						 || ( lpsz_column_name == "SEND_ID"              )
						)
					{
						continue;

					}

					if ( DT_STRING == dsType )
					{
						cmd.Parameters.Set(lpsz_column_name.ToLower(), sql_value.TrimOrBlank());
					}
					else 
					{
						cmd.Parameters.Set(lpsz_column_name.ToLower(), CDecimal::Parse(sql_value) );
					}
				}
			}
			cmd.SetCommandText(strSql);
			cmd.ExecuteNonQuery();

			Log::Trace("QMTI","f_qmtifa_ins","strSql={0}",strSql);

			// 记录结束时间
			gettimeofday(&tpend, NULL);
			// 计算时间差,毫秒
			timediff = (tpend.tv_sec - tpstart.tv_sec) * 1000 + (tpend.tv_usec - tpstart.tv_usec) * 0.001;

			sprintf(s.msg, "后台耗时[%.3f]毫秒", timediff);
	}

	/*捕获数据库操作异常*/
	catch(CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + strSql;
		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);	
		s.flag = -1;
		/*数据库异常时返回-1，事务将被回滚*/
		doFlag = -1;   
	}
	/*捕获应用错误*/
	catch(CApplicationException& ex)
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

	s.flag = doFlag;

	return(doFlag);
}
