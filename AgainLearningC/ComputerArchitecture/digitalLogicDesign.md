# Digital Logic Design

## Official Documentation
- **IEEE Standard for SystemVerilog—Unified Hardware Design, Specification, and Verification Language**
  - [Link](https://ieeexplore.ieee.org/document/8299595)

## Research Papers
- **"A Survey of Digital Logic Simulators" by R. B. Jones and V. H. Allan (1990)**
  - [Link](https://ieeexplore.ieee.org/document/55686)
- **"A Survey of High-Level Synthesis Techniques for Area Minimization" by S. Ahuja et al. (2018)**
  - [Link](https://ieeexplore.ieee.org/document/8481667)

## Additional Online Resources
- **Neso Academy: Digital Electronics**
  - [Link](https://www.youtube.com/playlist?list=PLBlnK6fEyqRjMH3mWf6kwqiTbT798eAOm)
- **All About Circuits: Digital Logic**
  - [Link](https://www.allaboutcircuits.com/textbook/digital/)
- **MIT OpenCourseWare: 6.004 Computation Structures**
  - [Link](https://ocw.mit.edu/courses/6-004-computation-structures-spring-2017/)

## Books
- **"Digital Design: With an Introduction to the Verilog HDL" by M. Morris Mano and Michael D. Ciletti**
  - ISBN: 0134549899
- **"Digital Design and Computer Architecture" by David Harris and Sarah Harris**
  - ISBN: 0123944244
- **"Fundamentals of Digital Logic with Verilog Design" by Stephen Brown and Zvonko Vranesic**
  - ISBN: 0073380547

## For Deep Dive into Implementation
- **"FPGA Prototyping by Verilog Examples: Xilinx Spartan-3 Version" by Pong P. Chu**
  - ISBN: 0470185325

## Standards
- **IEEE 1364-2005 - IEEE Standard for Verilog Hardware Description Language**
  - Not freely available, but it's the standard for Verilog HDL

## Code Example (Verilog)
```verilog
module d_flip_flop (
    input clk,
    input d,
    output reg q
);

always @(posedge clk) begin
    q <= d;
end

endmodule

module counter (
    input clk,
    input reset,
    output reg [3:0] count
);

always @(posedge clk or posedge reset) begin
    if (reset)
        count <= 4'b0000;
    else
        count <= count + 1;
end

endmodule
```

## Note
Digital Logic Design is fundamental to understanding how modern digital systems work. It covers both combinational and sequential circuits, which form the building blocks of more complex digital systems like processors and memory units.
