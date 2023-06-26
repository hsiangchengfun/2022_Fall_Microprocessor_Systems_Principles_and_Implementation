`timescale 1ns / 1ps


module profile (
    input clk_i,
    input rst_i,
    input stall_i,
    input [32-1:0] pc_i,
    input [32-1:0] instruction_i
);


// core_bench_list()
// core_list_find()
// matrix_mul_matrix_vitextract()
// core_state_transition()
// crcu16()

reg [64-1:0] total_cycle_counter,core_init_state_counter;
reg [64-1:0] core_bench_list_counter;
reg [64-1:0] core_list_find_counter;
reg [64-1:0] matrix_mul_matrix_vitextract_counter;
reg [64-1:0] core_state_transition_counter;
reg [64-1:0] crcu16_counter;

localparam [3-1:0] S_MAIN_INIT = 3'b000, S_MAIN_WAIT = 3'b001,
                   S_MAIN_READ = 3'b010, S_MAIN_DONE = 3'b011;

reg [3-1:0] P,P_next;
reg [3-1:0] function_collected;






always @(posedge clk_i ) begin
    if(rst_i)begin
        P <= S_MAIN_INIT;
    end
    else begin
        P <= P_next;
    end

end

always @(posedge clk_i ) begin
    case(P)
        S_MAIN_INIT:
            P_next <= S_MAIN_WAIT;
        S_MAIN_WAIT:
            P_next <= S_MAIN_READ;
        S_MAIN_READ:
            P_next <= (function_collected == 'd5)? S_MAIN_DONE : S_MAIN_READ;
        S_MAIN_DONE:
            P_next <= S_MAIN_INIT;
    endcase

end

wire total_cycle_flag,core_init_state_flag,core_bench_list_flag,core_list_find_flag,core_state_transition_flag,matrix_mul_matrix_vitextract_flag,crcu16_flag;

reg startsignal,endsignal;
always @(posedge clk_i ) begin
    if(rst_i)begin
        startsignal <= 0;
        endsignal <= 0;
    end
    // else begin
    //     if(pc_i == 32'h00001088) begin
    //         startsignal <= 1;
    //     end
    //     if(pc_i == 32'h00003750)begin
    //         endsignal <= 1;
    //     end

    // end
    else begin
        if(pc_i == 32'h000000b8) begin
            startsignal <= 1;
        end
        if(pc_i == 32'h0000270c)begin
            endsignal <= 1;
        end

    end
end


assign total_cycle_flag = ( startsignal && !endsignal);
assign core_init_state_flag = (pc_i >= 32'h000028b0 && pc_i <= 32'h000029f4 && total_cycle_flag);
assign core_bench_list_flag = (pc_i >= 32'h00001ed8 && pc_i <= 32'h0000210c && total_cycle_flag);
assign core_list_find_flag = (pc_i >= 32'h000001d10 && pc_i <= 32'h00001d64 && total_cycle_flag);
assign core_state_transition_flag =  (pc_i >= 32'h000029f8 && pc_i <= 32'h00002d10 && total_cycle_flag);
assign matrix_mul_matrix_vitextract_flag = (pc_i >= 32'h00002658 && pc_i <= 32'h00002714 && total_cycle_flag);
assign crcu16_flag = (pc_i >= 32'h00001a00 && pc_i <= 32'h00001a2c && total_cycle_flag);
            



always @(posedge clk_i ) begin
    if(rst_i)begin
        total_cycle_counter <= 0;
        core_init_state_counter <= 0;
        core_bench_list_counter <= 0;
        core_list_find_counter <= 0;
        core_state_transition_counter <= 0;
        matrix_mul_matrix_vitextract_counter <= 0;
        crcu16_counter <= 0;
    end
    else begin
        if(total_cycle_flag)total_cycle_counter <= total_cycle_counter + 'd1;
        if(core_init_state_flag)core_init_state_counter <= core_init_state_counter + 'd1;
        if(core_bench_list_flag)core_bench_list_counter <= core_bench_list_counter + 'd1;
        if(core_list_find_flag)core_list_find_counter <= core_list_find_counter + 1;
        if(core_state_transition_flag)core_state_transition_counter <= core_state_transition_counter + 1;
        if(matrix_mul_matrix_vitextract_flag) matrix_mul_matrix_vitextract_counter <= matrix_mul_matrix_vitextract_counter + 1;
        if(crcu16_flag)crcu16_counter <= crcu16_counter + 1;
    end
end








wire [32-1 : 0] rv32_instr = instruction_i;
wire [ 6 : 0]     opcode = rv32_instr[6: 0];

wire opcode_4_2_000 = (opcode[4: 2] == 3'b000);
wire opcode_6_5_00 = (opcode[6: 5] == 2'b00);
wire opcode_6_5_01 = (opcode[6: 5] == 2'b01);

wire rv32_load = opcode_6_5_00 & opcode_4_2_000;    // LOAD opcode
wire rv32_store = opcode_6_5_01 & opcode_4_2_000;   // STORE opcode
wire memory_flag = ( rv32_load || rv32_store );


(* mark_debug = "true" *) reg [32-1:0] core_bench_list_counter_memory;
(* mark_debug = "true" *) reg [32-1:0] core_list_find_counter_memory;
(* mark_debug = "true" *) reg [32-1:0] matrix_mul_matrix_vitextract_counter_memory;
(* mark_debug = "true" *) reg [32-1:0] core_state_transition_counter_memory;
(* mark_debug = "true" *) reg [32-1:0] crcu16_counter_memory;

// (* mark_debug = "true" *) reg [32-1:0] core_bench_list_counter_mem_stall;
// (* mark_debug = "true" *) reg [32-1:0] core_list_find_counter_mem_stall;
// (* mark_debug = "true" *) reg [32-1:0] matrix_mul_matrix_vitextract_counter_mem_stall;
// (* mark_debug = "true" *) reg [32-1:0] core_state_transition_counter_mem_stall;
// (* mark_debug = "true" *) reg [32-1:0] crcu16_counter_mem_stall;


(* mark_debug = "true" *) reg [32-1:0] core_bench_list_counter_stall;
(* mark_debug = "true" *) reg [32-1:0] core_list_find_counter_stall;
(* mark_debug = "true" *) reg [32-1:0] matrix_mul_matrix_vitextract_counter_stall;
(* mark_debug = "true" *) reg [32-1:0] core_state_transition_counter_stall;
(* mark_debug = "true" *) reg [32-1:0] crcu16_counter_stall;




always @(posedge clk_i ) begin
    if(rst_i)begin
        
        core_bench_list_counter_memory <= 0;
        core_list_find_counter_memory <= 0;
        core_state_transition_counter_memory <= 0;
        matrix_mul_matrix_vitextract_counter_memory <= 0;
        crcu16_counter_memory <= 0;

        
        core_bench_list_counter_stall <= 0;
        core_list_find_counter_stall <= 0;
        core_state_transition_counter_stall <= 0;
        matrix_mul_matrix_vitextract_counter_stall <= 0;
        crcu16_counter_stall <= 0;


        // core_bench_list_counter_mem_stall <= 0;
        // core_list_find_counter_mem_stall <= 0;
        // core_state_transition_counter_mem_stall <= 0;
        // matrix_mul_matrix_vitextract_counter_mem_stall <= 0;
        // crcu16_counter_mem_stall <= 0;
    end
    else begin
        
        if(core_bench_list_flag && memory_flag  )core_bench_list_counter_memory <= core_bench_list_counter_memory + 'd1;
        if(core_list_find_flag && memory_flag  )core_list_find_counter_memory <= core_list_find_counter_memory + 1;
        if(core_state_transition_flag && memory_flag )core_state_transition_counter_memory <= core_state_transition_counter_memory + 1;
        if(matrix_mul_matrix_vitextract_flag && memory_flag ) matrix_mul_matrix_vitextract_counter_memory <= matrix_mul_matrix_vitextract_counter_memory + 1;
        if(crcu16_flag && memory_flag )crcu16_counter_memory <= crcu16_counter_memory + 1;
    end
    
    if (stall_i) begin
        
        if(core_bench_list_flag && !memory_flag )core_bench_list_counter_stall <= core_bench_list_counter_stall + 'd1;
        if(core_list_find_flag && !memory_flag )core_list_find_counter_stall <= core_list_find_counter_stall + 1;
        if(core_state_transition_flag && !memory_flag )core_state_transition_counter_stall <= core_state_transition_counter_stall + 1;
        if(matrix_mul_matrix_vitextract_flag && !memory_flag ) matrix_mul_matrix_vitextract_counter_stall <= matrix_mul_matrix_vitextract_counter_stall + 1;
        if(crcu16_flag && !memory_flag  )crcu16_counter_stall <= crcu16_counter_stall + 1;

        

    end
    
    
end









    
endmodule