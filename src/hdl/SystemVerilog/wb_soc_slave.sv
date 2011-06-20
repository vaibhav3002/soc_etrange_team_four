module wb_soc_slave (
                     input  p_clk,
                     input  p_resetn,
		     input  logic raise_irq,
		     output logic irq,
		     output logic [31:0] module_register,
		     output logic initiliazed,
                     //  WB signals
                     input  [31:0] p_wb_DAT_I,
                     output [31:0] p_wb_DAT_O,
                     input  [31:0] p_wb_ADR_I,
                     output p_wb_ACK_O,
                     input  p_wb_CYC_I,
                     output p_wb_ERR_O,
                     input  p_wb_LOCK_I,
                     output p_wb_RTY_O,
                     input  [3:0]p_wb_SEL_I,
                     input  p_wb_STB_I,
                     input  p_wb_WE_I 
                     );

   integer 		    cycle;

   // always acknowledge requests
   assign p_wb_ACK_O = p_wb_CYC_I && p_wb_STB_I;
   // read write request counter
   assign p_wb_DAT_O = module_register;

   assign p_wb_ERR_O = 1'b0;
   assign p_wb_RTY_O = 1'b0;

   always_ff @(posedge p_clk)
     begin : transition
	if (!p_resetn)
	  begin
	     cycle = 0;
	     initiliazed <= 1'b0;
	  end
	else
	  cycle++;
	if(((!p_wb_STB_I)||(!p_wb_CYC_I)||(!p_wb_WE_I))&&(raise_irq))
		irq <= 1'b1;

	if (p_wb_STB_I ) 
	  begin
             if (p_wb_CYC_I ) 
               begin
		  if (p_wb_WE_I) 
		    begin
		       //pragma translate_off
		       $display ("SLAVE recieved Write REQ 0x%08X @cycle %d",p_wb_DAT_I,cycle);
		       //pragma translate_on
		       module_register <= p_wb_DAT_I;
		       initiliazed <= 1'b1;
		       irq <= 1'b0;		
		    end
		  else
		    begin
		       //pragma translate_off
		       $display ("SLAVE recieved Read request @cycle %d",cycle);
		       //pragma translate_on
		    end
               end
             else 
               begin
		  //pragma translate_off
		  $display ("SLAVE recieved invalid STROBE @ cycle %d",cycle);
		  //pragma translate_on
               end
	  end
     end : transition


endmodule
