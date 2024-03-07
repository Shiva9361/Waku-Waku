<template>
  <div class="root">
    <h1>Waku-Waku Simulator</h1>
    <i @click="postRunRequest" class="fa fa-play" aria-hidden="true"></i>
    <i @click="play_pause" class="fa fa-play" aria-hidden="true"></i>
    <form method="post">
      <div>
        <label>Addi</label>
        <input type="number" id="addi" value="1" min="1" />
      </div>
      <div>
        <label>Add</label>
        <input type="number" id="add" value="1" min="1" />
      </div>
      <div>
        <label>Sub</label>
        <input type="number" id="sub" value="1" min="1" />
      </div>
      <div>
        <label>Mul</label>
        <input type="number" id="mul" value="1" min="1" />
      </div>
      <div>
        <label>Div</label>
        <input type="number" id="div" value="1" min="1" />
      </div>
    </form>
    <div class="content">
      <PipelineHistory
        :pipeline_history="pipeline_history"
        :counter="counter"
      />
      <RegisterHistory
        :register_history="register_history"
        :counter="counter"
      />
    </div>
    <textarea class="text" id="code0" rows="10" cols="10"> </textarea>
  </div>
</template>

<script>
import PipelineHistory from "./components/PipelineHistory.vue";
import RegisterHistory from "./components/RegisterHistory.vue";
import axios from "axios";
export default {
  name: "App",
  data() {
    return {
      pipeline_history: [],
      register_history: [],
      counter: 0,
      play: false,
    };
  },
  components: {
    PipelineHistory,
    RegisterHistory,
  },
  methods: {
    async fetchPipelineHistory() {
      const res = await fetch("http://localhost:5000/core/0/pipe");
      const data = await res.json();
      return data, data;
    },
    async fetchRegisterHistory() {
      const res = await fetch("http://localhost:5000/core/0/reg");
      const data = await res.json();
      return data;
    },
    async postRunRequest() {
      try {
        const formData = axios.toFormData({
          pipeline: "true",
          forward: "true",
          addi: document.getElementById("addi").value,
          add: document.getElementById("add").value,
          div: document.getElementById("div").value,
          sub: document.getElementById("sub").value,
          mul: document.getElementById("mul").value,
          code0: document.getElementById("code0").value,
        });
        await axios.post("http://localhost:5000/run", formData, {
          headers: {
            "Content-Type": "multipart/form-data",
          },
        });
        this.register_history = await this.fetchRegisterHistory();
        this.pipeline_history = await this.fetchPipelineHistory();
        this.counter = 0;
        this.play = true;
      } catch (error) {
        console.log(error);
      }
    },
    play_pause() {
      this.play = !this.play;
    },
  },
  created() {
    setInterval(() => {
      if (this.play) this.counter++;
      if (this.counter == this.pipeline_history.length - 1) this.play = false; // make it non playable after this
    }, 1000);
  },
};
</script>

<style scoped>
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}
h1 {
  text-align: center;
}
.text {
  margin-left: 30px;
  width: 500px;
  height: 500px;
  display: inline-block;
}
.content {
  display: inline-block;
}
</style>
