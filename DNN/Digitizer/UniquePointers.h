OK. ./DigitizingEngine.h:62:        virtual std::map<Int_t, std::unique_ptr<Paddle>> ExtractPaddles() = 0;
OK. ./DigitizingTacQuila.h:144:        std::map<Int_t, std::unique_ptr<DigitizingEngine::Paddle>> ExtractPaddles() override;
OK. ./DigitizingTacQuila.h:147:        std::map<Int_t, std::unique_ptr<Paddle>> paddles;
./R3BNeulandCADDigitizer.h:100:    std::unique_ptr<DigitizingEngine> fDigitizingEngine; // owning (sink)
./R3BNeulandCADDigitizer.h:108:    std::unique_ptr<TClonesArray> fNeulandDigis;  // owning
./R3BNeulandCADDigitizer.h:109:    std::unique_ptr<TClonesArray> fNeulandPixels; // owning
OK. ./DigitizingTacQuila.cxx:227:            paddles[paddle_id] = std::unique_ptr<Paddle>(new Paddle(par));
OK. ./DigitizingTacQuila.cxx:252:    std::map<Int_t, std::unique_ptr<DigitizingEngine::Paddle>> DigitizingTacQuila::ExtractPaddles()
OK. ./DigitizingTacQuila.cxx:254:        std::map<Int_t, std::unique_ptr<DigitizingEngine::Paddle>> out;
OK. ./DigitizingTacQuila.cxx:257:            // std::unique_ptr<DigitizingTacQuila::Paddle> -> std::unique_ptr<DigitizingEngine::Paddle>
./R3BNeulandCADDigitizer.cxx:367:            std::unique_ptr<R3BNeulandDigi> digi(new R3BNeulandDigi(paddleID,
